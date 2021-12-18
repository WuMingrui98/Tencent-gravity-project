import pygame
from pygame.locals import *

pygame.init()


class Santa(pygame.sprite.Sprite):
    def __init__(self, bg_size):
        super().__init__()
        self.image = []
        self.rect = []
        self.mask = []
        self.width, self.height = bg_size
        for i in range(1, 10):
            self.image.append(pygame.image.load(r'../images/圣诞老人' + str(i) + '.png').convert_alpha())
        self.rect = self.image[4].get_rect()
        self.mask = pygame.mask.from_surface(self.image[4])
        self.rect.left, self.rect.top = (self.width - self.rect.width) // 2, self.height - self.rect.height - 40
        self.speed = 6
        self.active = True
        self.direction = True  # True 是右边，False是左边

    def move_left(self):
        if self.direction:
            for i in range(9):
                self.image[i] = pygame.transform.flip(self.image[i], True, False)
            self.direction = False
        if self.rect.left > 0:
            self.rect.left -= self.speed
        else:
            self.rect.left = 0

    def move_right(self):
        if not self.direction:
            for i in range(9):
                self.image[i] = pygame.transform.flip(self.image[i], True, False)
            self.direction = True
        if self.rect.right < self.width:
            self.rect.right += self.speed
        else:
            self.rect.right = self.width

    def reset(self):
        self.active = True
        if not self.direction:
            for i in range(9):
                self.image[i] = pygame.transform.flip(self.image[i], True, False)
            self.direction = True
        self.rect.left, self.rect.top = (self.width - self.rect.width) // 2, self.height - self.rect.height - 40
