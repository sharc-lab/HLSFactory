from cv2 import sqrt
import numpy as np
import cv2 as cv

import matplotlib.pyplot as plt

cap = cv.VideoCapture(cv.samples.findFile("Input/test2.avi"))
ret, frame1 = cap.read()
frame1 = cv.imread("./car1.bmp")
frame2 = cv.imread("./car2.bmp")
prvs = cv.cvtColor(frame1, cv.COLOR_BGR2GRAY)
hsv = np.zeros_like(frame1)
hsv[..., 1] = 255



count = 0

while(1):
    # ret, frame2 = cap.read()
    # if not ret:
    #     print('No frames grabbed!')
    #     break
    next = cv.cvtColor(frame2, cv.COLOR_BGR2GRAY)
    # Saving Input frames (Grayscale) as np arrays in CSV format
    # np.savetxt('Output/InputFramesGS/' + str(count) + '.csv', np.asarray(next))
    # Saving Gaussian blurred frames as np arrays in CSV format (BORDER_DEFAULT)
    # ksize = 5
    # np.savetxt('Output/BlurredFrames/' + str(count) + '.csv', np.asarray(cv.GaussianBlur(next,(ksize,ksize),0)))
    
    flow = cv.calcOpticalFlowFarneback(prvs, next, None, 0.99999, 1, 11, 1, 5, 1.1, 0)
    # print(flow[..., 0])
    # Saving flow vectors computed by dense optical flow
    # np.savetxt('Output/xFlowVectorFrames/' + str(count) + '.csv', np.asarray(flow[..., 0]))
    # np.savetxt('Output/yFlowVectorFrames/' + str(count) + '.csv', np.asarray(flow[..., 1]))
    # count = count + 1
    mag, ang = cv.cartToPolar(flow[..., 0], flow[..., 1])
    hsv[..., 0] = ang*180/np.pi/2
    hsv[..., 2] = cv.normalize(mag, None, 0, 255, cv.NORM_MINMAX)
    bgr = cv.cvtColor(hsv, cv.COLOR_HSV2BGR)
    # th, dst = cv.threshold(bgr, 30, 255, cv.THRESH_TOZERO)
    # cv.imshow('Frame', frame2)
    # cv.imshow('Flow Map', bgr)
    dense_flow = cv.addWeighted(frame2, 1,bgr, 2, 0)
    
    h, w = flow.shape[:2]
    print(h,w)
    flow = -flow
    flow[...,0] += np.arange(w)
    flow[...,1] += np.arange(h)[:,np.newaxis]
    warpedImage = cv.remap(next, flow, None, cv.INTER_LINEAR)
    # black_pixels = np.where(
    #     (bgr[:, :, 0] == 0) & 
    #     (bgr[:, :, 1] == 0) & 
    #     (bgr[:, :, 2] == 0)
    # )
    # bgr[black_pixels] = [255, 255, 255]
    cv.imshow("Dense optical flow", dense_flow)
    cv.imshow("Warped Image", warpedImage)
    f1 = open ( 'ref_C_Vx.txt' , 'r')
    f2 = open ( 'ref_C_Vy.txt' , 'r')
    # Vx = []
    # Vx = ([ line.split() for line in f1])
    # for i,line in enumerate(Vx):
    #     Vx[i] =  [(float(ele)) for ele in line]
    # Vy = []
    # Vy = ([ line.split() for line in f2])
    # for i,line in enumerate(Vy):
    #     Vy[i] =  [(float(ele)) for ele in line]
    
    Vx = []    
    for line in f1:
        temp = ( line.split())
        Vx.append( [(float(ele)) for ele in temp])
    Vy = []
    for line in f2:
        temp = (line.split())
        Vy.append( [(float(ele)) for ele in temp])
    
    Vx = np.asarray(Vx)
    Vy = np.asarray(Vy)
    
    # infoVx = np.iinfo(Vx.dtype) # Get the information of the incoming image type
    #maxVx = Vx.max
    #Vx = Vx/maxVx # normalize the data to 0 - 1
    # Vx = 255 * Vx # Now scale by 255
    # Vx = Vx.astype(np.uint8)
    #maxVy = Vy.max
    # infoVy = np.iinfo(Vy.dtype) # Get the information of the incoming image type
    #Vy = Vy / maxVy # normalize the data to 0 - 1
    # Vy = 255 * Vy # Now scale by 255
    # Vy = Vy.astype(np.uint8)
    # flow_C = Vx+Vy
    print(np.asarray(Vx).shape, np.asarray(Vy).shape)
    hsv1 = np.zeros_like(frame1)
    hsv1[..., 1] = 255
    mag1, ang1 = cv.cartToPolar(np.asarray(Vx), np.asarray(Vy))
    hsv1[..., 0] = ang1*180/np.pi/2
    hsv1[..., 2] = cv.normalize(mag1, None, 0, 255, cv.NORM_MINMAX)
    hsv1[..., 2] *= 5
    bgr1 = cv.cvtColor(hsv1, cv.COLOR_HSV2BGR)   
    # bgr1 = cv.convertScaleAbs(bgr1)   
    
    dense_flow1 = cv.addWeighted(frame2, 1,bgr1, 2, 0)
    # get (i, j) positions of all RGB pixels that are black (i.e. [0, 0, 0])
    # black_pixels = np.where(
    #     (bgr1[:, :, 0] == 0) & 
    #     (bgr1[:, :, 1] == 0) & 
    #     (bgr1[:, :, 2] == 0)
    # )

    # # set those pixels to white
    # bgr1[black_pixels] = [255, 255, 255]
    Opticflow = np.zeros_like(flow)
    Opticflow[...,0]=np.asarray(Vx)
    Opticflow[...,1]=np.asarray(Vy)
    
    # Opticflow = flow_C
    h, w = Opticflow.shape[:2]
    Opticflow = -Opticflow
    Opticflow[...,0] += np.arange(w)
    Opticflow[...,1] += np.arange(h)[:,np.newaxis]
    warpedImageC = cv.remap(next, Opticflow, None, cv.INTER_LINEAR)
    
    cv.imshow("optical flow C", dense_flow1)
    cv.imshow("Warped Image C", warpedImageC)
    
    cv.imwrite("overlayed_optical_flow_C.png", dense_flow1)
    cv.imwrite("warped_image_C.png", warpedImageC)
    cv.imwrite("overlayed_optical_flow_python.png", dense_flow)
    cv.imwrite("warped_image_python.png", warpedImage)
    
    cv.imwrite("optical_flow_C.png", bgr1)
    cv.imwrite("optical_flow_python.png", bgr)
    # print(Vx,Vy)
    # # Saving flow color map computed by dense optical flow
    # np.savetxt('Output/FlowMapFrames/' + str(count) + '.csv', np.asarray(bgr))
    # Press ESC to stop processing or wait till video ends
    if cv.waitKey(0) & 0xFF == ord('q'):
        break
    # k = cv.waitKey(30) & 0xff
    # if k == 27:
    #     break
    # prvs = next
cv.destroyAllWindows()
