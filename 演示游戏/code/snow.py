import pygame
from pygame.locals import *
from random import *

pygame.init()


class Snow:
    def __init__(self, bg_size):
        self.image = pygame.image.load(r'../images/雪花.png').convert_alpha()
        self.image = pygame.transform.scale(self.image, (10, 10))
        self.speed = 3
        self.width, self.height = bg_size
        self.rect = self.image.get_rect()
        self.rect.left = 10 * randint(0, 49)
        self.rect.top = -50 * randint(0, 14)

    def move(self):
        if self.rect.top > self.height:
            self.reset()
        else:
            self.rect.top += self.speed

    def reset(self):
        self.rect.left = 10 * randint(0, 49)
        self.rect.top = -50 * randint(0, 14)
