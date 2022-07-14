import cv2
import os
import numpy as np

from utils.RGBDSender import RGBDData, RGBDSender

sender = RGBDSender()
sender.open("./pipe_dir/pipe2")
# cv2.namedWindow("123", cv2.WINDOW_NORMAL)
# cv2.resizeWindow("123", (960, 540))

for i in range(5):
    imgs = [cv2.imread(f"/data/GoPro/videos/teaRoom/sequence/video/{j + 1}-{i * 5 + 101}.png") for j in range(5)]
    masks = [cv2.imread(f"/data/GoPro/videos/teaRoom/sequence/mask/{j + 1}-{i * 5 + 101}.png", cv2.IMREAD_GRAYSCALE) for
             j in range(5)]
    depths = []
    for j in range(5):
        with open(f"/data/GoPro/videos/teaRoom/sequence/depth/{i * 5 + 101:04d}/{j + 1:04d}.pfm", "rb") as f:
            depth = f.read()[22:]
        d = np.zeros((912, 1600), dtype=np.float32)
        depth = np.frombuffer(depth, np.float32).reshape(896, 1600)[::-1]
        d[8:904] = depth
        d = cv2.resize(d, (3840, 2160), cv2.INTER_LINEAR)
        depths.append(d)
        # cv2.imshow("123", depth)
        # cv2.waitKey(0)
        # cv2.imshow("123", d)
        # cv2.waitKey(0)
    print([i.shape for i in imgs])
    print([i.shape for i in masks])
    print([i.shape for i in depths])
    h, w, _ = imgs[0].shape
    x, y = 0, 0
    data = RGBDData(5, imgs, depths, masks, [(w, h, 0, 0) for j in range(5)])
    sender.sendData(data)
input()
sender.close()
