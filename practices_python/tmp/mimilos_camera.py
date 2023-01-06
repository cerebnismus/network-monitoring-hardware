#!/usr/local/bin/python3
# -*- coding: utf-8 -*-
#  -*- cerebnismus -*-
 
import cv2
import numpy as np

cap = cv2.VideoCapture(0)
# cap = cv2.VideoCapture('http://114.185.129.202/-wvhttp-01-/GetOneShot?image_size=640x480&frame_count=1000000000')

while True:
    ret, frame = cap.read()
    frame_width = frame.shape[1]
    frame_height = frame.shape[0]

    # convert to blob
    frame_blob = cv2.dnn.blobFromImage(
        frame, 1/255, (416, 416), swapRB=True, crop=False)

    # set of 80 class labels
    labels = ["person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
              "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant", "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"]

    # colors to represent each label uniquely
    colors = ["0,255,255", "0,0,255", "255,0,0", "255,255,0", "0,255,0"]
    colors = [np.array(color.split(",")).astype("int") for color in colors]
    colors = np.array(colors)
    colors = np.tile(colors, (18, 1))

    # load pre-trained model
    model = cv2.dnn.readNet("/Users/macbook/Desktop/Python/yolov3-tiny.cfg", "/Users/macbook/Desktop/Python/yolov3-tiny.weights")
    layers = model.getLayerNames()
    output_layer = [layers[layer-1]
                    for layer in model.getUnconnectedOutLayers()]

    model.setInput(frame_blob)
    detection_layers = model.forward(output_layer)

    ids_list = []
    boxes_list = []
    confidences_list = []

    for detection_layer in detection_layers:
        for object_detection in detection_layer:
            # obj_detections[1 to 4] => will have the two center points, box width and box height
            # obj_detections[5] => will have scores for all objects within bounding box
            scores = object_detection[5:]
            predicted_id = np.argmax(scores)
            confidence = scores[predicted_id]

            # take only predictions with confidence more than 20%
            if confidence > 0.20:
                label = labels[predicted_id]
                bounding_box = object_detection[0:4] * np.array(
                    [frame_width, frame_height, frame_width, frame_height])
                (box_center_x_pt, box_center_y_pt, box_width,
                 box_height) = bounding_box.astype("int")
                start_x_pt = int(box_center_x_pt - (box_width / 2))
                start_y_pt = int(box_center_y_pt - (box_height / 2))

                # save id, start x, start y, width & height, confidences in a list for nms processing
                ids_list.append(predicted_id)
                confidences_list.append(float(confidence))
                boxes_list.append(
                    [start_x_pt, start_y_pt, int(box_width), int(box_height)])

    max_ids = cv2.dnn.NMSBoxes(boxes_list, confidences_list, 0.5, 0.4)

    for max_id in max_ids:
        max_class_id = max_id
        box = boxes_list[max_class_id]

        start_x_pt = box[0]
        start_y_pt = box[1]
        box_width = box[2]
        box_height = box[3]

        predicted_id = ids_list[max_class_id]
        label = labels[predicted_id]
        confidence = confidences_list[max_class_id]

        end_x_pt = start_x_pt + box_width
        end_y_pt = start_y_pt + box_height

        box_color = colors[predicted_id]
        box_color = [int(each) for each in box_color]

        label = "{}: {:.2f}%".format(label, confidence*100)
        print("Predicted Object -> {}".format(label))

        # if predicted object is person, 
        # more than 90% confident and more detect than 5 times
        if predicted_id == 0 and confidence > 0.5:
            cv2.rectangle(frame, (start_x_pt, start_y_pt),
                              (end_x_pt, end_y_pt), box_color, 1)
            cv2.putText(frame, label, (start_x_pt, start_y_pt-5),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.9, box_color, 1)
                
            # stop the motor
            print("\n\t - PERSON DETECTED - \n")
            cv2.imshow("Mimilos v1 - Detection Output", frame)

        cv2.rectangle(frame, (start_x_pt, start_y_pt),
				              (end_x_pt, end_y_pt), box_color, 1)
        cv2.putText(frame, label, (start_x_pt, start_y_pt-5),
				            cv2.FONT_HERSHEY_SIMPLEX, 0.5, box_color, 1)

    cv2.imshow("Mimilos v1 - Detection Window", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
      break
 
cap.release()
cv2.destroyAllWindows()
