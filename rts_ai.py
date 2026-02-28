import cv2
import serial
import time
from ultralytics import YOLO

model = YOLO("best.pt")
arduino = serial.Serial("COM3", 9600, timeout=1)
time.sleep(2)


def send(cmd):
    arduino.write(cmd.encode())
    time.sleep(0.05)


STATE_DRIVING = "DRIVING"
STATE_DRILLING = "DRILLING"
STATE_SCOOPING = "SCOOPING"

state = STATE_DRIVING
state_start = time.time()
last_cmd = None


def send_if_changed(cmd):
    global last_cmd
    if cmd != last_cmd:
        send(cmd)
        last_cmd = cmd


cap = cv2.VideoCapture(0)
width, height = 640, 480

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.resize(frame, (width, height))
    cell_w, cell_h = width // 4, height // 4

    for i in range(1, 4):
        cv2.line(frame, (0, i * cell_h), (width, i * cell_h), (80, 80, 80), 1)
        cv2.line(frame, (i * cell_w, 0), (i * cell_w, height), (80, 80, 80), 1)

    results = model(frame, stream=True)
    danger_zone_detected = False
    module_detected = False

    for r in results:
        for box in r.boxes:
            cls = int(box.cls[0])
            label = model.names[cls]
            conf = float(box.conf[0])
            if conf < 0.4:
                continue
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            cx, cy = (x1 + x2) // 2, (y1 + y2) // 2

            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(
                frame,
                f"{label} {conf:.2f}",
                (x1, y1 - 5),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.6,
                (255, 255, 255),
                1,
            )

            if cell_h * 2 < cy < cell_h * 3:
                if label in ["rock", "obstacle"]:
                    danger_zone_detected = True
                elif label == "module":
                    module_detected = True

    now = time.time()
    elapsed = now - state_start

    if state == STATE_DRIVING:
        if module_detected:
            send("S")
            send("B")
            state = STATE_DRILLING
            state_start = now
        elif danger_zone_detected:
            send_if_changed("L")
        else:
            send_if_changed("F")

    elif state == STATE_DRILLING:
        if elapsed >= 3.0:
            send("b")
            send("D")
            state = STATE_SCOOPING
            state_start = now

    elif state == STATE_SCOOPING:
        if elapsed >= 2.0:
            send("U")
            send("F")
            state = STATE_DRIVING
            state_start = now

    cv2.putText(
        frame,
        f"State: {state}",
        (10, 20),
        cv2.FONT_HERSHEY_SIMPLEX,
        0.6,
        (0, 200, 255),
        2,
    )

    cv2.imshow("Mars Rover Neural Vision", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
arduino.close()
cv2.destroyAllWindows()
