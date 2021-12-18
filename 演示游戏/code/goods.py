import pygame
from random import *
from pygame.locals import *

pygame.init()


class Candy(pygame.sprite.Sprite):
    def __init__(self, bg_size):
        super().__init__()
        self.image = pygame.image.load(r'../images/糖果.png').convert_alpha()
        self.image = pygame.transform.scale(self.image, (25, 50))
        self.rect = self.image.get_rect()
        self.width, self.height = bg_size[0], bg_size[1]
        self.rect.left, self.rect.top = self.rect.width * randint(0, (self.width - self.rect.width) // self.rect.width), \
                                        randint(-self.height, 0)
        self.speed = 2
        self.mask = pygame.mask.from_surface(self.image)
        self.active = True

    def move(self):
        self.rect.top += self.speed
        if self.rect.top > self.height:
            self.active = False

    def reset(self):
        self.rect.left, self.rect.top = self.rect.width * randint(0, (self.width - self.rect.width) // self.rect.width), \
                                        randint(-self.height, 0)
        self.active = True


class Present(pygame.sprite.Sprite):
    def __init__(self, bg_size):
        super().__init__()
        self.image = pygame.image.load(r'../images/礼物.png').convert_alpha()
        self.image = pygame.transform.scale(self.image, (50, 50))
        self.rect = self.image.get_rect()
        self.width, self.height = bg_size[0], bg_size[1]
        self.rect.left, self.rect.top = randint(0, self.width - self.rect.width), \
                                        2 * self.height + randint(-self.height, 0)
        self.speed = 5
        self.mask = pygame.mask.from_surface(self.image)
        self.active = False

    def move(self):
        self.rect.top += self.speed
        if self.rect.top > self.height:
            self.active = False

    def reset(self):
        self.rect.left, self.rect.top = randint(0, self.width - self.rect.width), \
                                        randint(-self.height, 0)
        self.active = True
