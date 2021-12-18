import pygame
from random import *
from pygame.locals import *

pygame.init()


class Bomb(pygame.sprite.Sprite):
    def __init__(self, bg_size):
        super().__init__()
        self.image = pygame.image.load(r'../images/炸弹.png').convert_alpha()
        self.image = pygame.transform.scale(self.image, (50, 50))
        self.rect = self.image.get_rect()
        self.width, self.height = bg_size
        self.rect.left, self.rect.top = self.rect.width * randint(0, (self.width - self.rect.width) // self.rect.width), \
                                        randint(-5 * self.height, 0)
        self.active = True
        self.speed = 2
        self.mask = pygame.mask.from_surface(self.image)

    def move(self):
        if self.rect.top > self.height:
            self.reset()
        else:
            self.rect.top += self.speed

    def reset(self):
        self.active = True
        self.rect.left, self.rect.top = self.rect.width * randint(0, (self.width - self.rect.width) // self.rect.width), \
                                        randint(-5 * self.height, 0)
