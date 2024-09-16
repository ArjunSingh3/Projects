
import os
import cv2
import numpy as np 
import sys
# import tensorflow as tf

import torch
import torch.nn as nn
import torch.nn.functional as F


import torch
import torchvision
import torchvision.transforms as transforms
from sklearn.model_selection import train_test_split

#Cuda
from numba import jit, cuda
import numpy as np
# to measure exec time
from timeit import default_timer as timer 

from torch.utils.data import Dataset
from torchvision import transforms
from PIL import Image

import torch.optim as optim

import matplotlib.pyplot as plt
import numpy as np


# Constants


EPOCHS = 2
IMG_WIDTH = 30
IMG_HEIGHT = 30
NUM_CATEGORIES = 43
TEST_SIZE = 0.4
BATCH_SIZE = 4


class TrafficSignDataset(Dataset):
    def __init__(self,data_dir, test_size, random_seed=42, train=False, test=False, validate = False, final_test=False, transform=None):
        self.data_dir = data_dir
        self.transform = transform
        self.test_size = test_size
        self.random_seed = random_seed
        self.train = train
        self.test = test
        self.validate = validate
        self.images, self.labels = self.load_data()
        # print('len of images array: ', len(self.labels))
        self.epochs = EPOCHS
        self.img_width = IMG_WIDTH
        self.img_height = IMG_HEIGHT
        self.num_categories = NUM_CATEGORIES
        self.batch_size = BATCH_SIZE
        self.final = final_test
        
        # self.total_labels = self.load_labels()

        # Split stage
        self.train_images, self.test_images, self.train_labels, self.test_labels = train_test_split(
            self.images, self.labels, test_size=self.test_size, random_state=self.random_seed
        )
        
        self.train_train_images, self.validate_images, self.train_train_labels, self.validate_labels = train_test_split(
            self.train_images, self.train_labels, test_size=self.test_size, random_state=self.random_seed
        )
        

        # print('OG:',len(self.train_images), len(self.train_labels))

        
    
    def load_labels():
        
        raise NotImplementedError
    def load_data(self):
        '''
            Attention!! Needs Implementation
        '''
        
        """
        Load image data from directory `data_dir`.

        Assume `data_dir` has one directory named after each category, numbered
        0 through NUM_CATEGORIES - 1. Inside each category directory will be some
        number of image files.

        Return tuple `(images, labels)`. `images` should be a list of all
        of the images in the data directory, where each image is formatted as a
        numpy ndarray with dimensions IMG_WIDTH x IMG_HEIGHT x 3. `labels` should
        be a list of integer labels, representing the categories for each of the
        corresponding `images`.
        """

        

        labels = []

        images = []

        for directory_name in os.listdir(self.data_dir):
            current = os.path.join(self.data_dir,directory_name)
            if os.path.isdir(current):
                for filename in os.listdir(current):
                    current_image = os.path.join(current,filename)

                    image = cv2.imread(current_image)

                    alteration = (30,30)

                    altered_photograph = cv2.resize(image,alteration)

                    image_array = np.array(altered_photograph)

                    labels.append(int(directory_name))
                    images.append(image_array)
        
        return images, labels

    def __len__(self) -> int:
        
        if self.train:    
            return len(self.train_train_images)
        elif self.validate:
            return len(self.validate_images)
        elif self.final:
            return len(self.images)
        else:
            return len(self.test_images)
    
    def __getitem__(self, index):
        # print(len(self.train_images), len(self.train_labels))
        # print('index value:', index)
        # print('value at that index: ', self.train_images[index])
        # print('maybe value here: ', self.test_images[index])

        if self.train:
            #print('index value train: ',index)
            image_t = self.train_train_images[index]
            label_t = self.train_train_labels[index]
        elif self.validate:
            image_t = self.validate_images[index]
            label_t = self.validate_labels[index]
        elif self.final:
            image_t = self.images[index]
            label_t = self.labels[index]
        else:
            #print('index value test: ',index)
            image_t = self.test_images[index]
            label_t = self.test_labels[index]
        '''
        Check the input format needed as this function will impact how the images 
        are inputted inside the neural network
        '''
        if self.transform:
            image = self.transform(image_t)
            # label = self.transform(label_t)

        return image, label_t
    
    def get_classes(self):
        return self.total_labels
        
