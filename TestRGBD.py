import cv2
import os
import numpy as np

from utils.RGBDSender import RGBDData, RGBDSender

sender = RGBDSender()
sender.open("./pipe_dir/pipe2")

for i in range(5):
    imgs = [cv2.imread(f"/data/GoPro/videos/teaRoom/sequence/video/{j+1}-{i * 5 + 101}.png") for j in range(5)]
    masks = [cv2.imread(f"/data/GoPro/videos/teaRoom/sequence/mask/{j+1}-{i * 5 + 101}.png", cv2.IMREAD_GRAYSCALE) for j in range(5)]
    print([i.shape for i in imgs])
    print([i.shape for i in masks])
    h, w, _ = imgs[0].shape
    x, y = 0, 0
    depths = [np.zeros((h, w), dtype=np.float32) for j in range(5)]
    data = RGBDData(5, imgs, depths, masks, [(w, h, 0, 0) for j in range(5)])
    sender.sendData(data)
input()
sender.close()
