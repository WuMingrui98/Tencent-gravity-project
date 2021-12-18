import sys
import traceback

import pygame
from pygame.locals import *

import bomb
import goods
import santa
import snow

pygame.init()
pygame.mixer.init()

bg_size = width, height = 500, 700
screen = pygame.display.set_mode(bg_size)
bg_image = pygame.image.load(r'../images/背景.png').convert()
bg_image = pygame.transform.scale(bg_image, (500, 700))

# 音乐
pygame.mixer.music.load(r'../sound/jingle bells.ogg')
pygame.mixer.music.set_volume(0.5)


def main():
    # 游戏帧数:
    frame_num = 60
    # 分数记录
    score = 0
    # 生成圣诞老人
    mysanta = santa.Santa(bg_size)
    # 生成雪
    mysnow = []
    for i in range(30):
        mysnow.append(snow.Snow(bg_size))
    # 生成炸弹
    mybomb = pygame.sprite.Group()
    for i in range(10):
        mybomb.add(bomb.Bomb(bg_size))
    # 生成糖果
    mycandy = pygame.sprite.Group()
    for i in range(5):
        mycandy.add(goods.Candy(bg_size))
    # 生成礼物
    mypresent = goods.Present(bg_size)
    # 建立Clock类
    clock = pygame.time.Clock()
    # 暂停操作
    pause = False
    # 游戏结束操作
    gameover = False
    # 建立延迟
    delay = 100
    # 载入暂停图片
    pause_nor_image = pygame.image.load(r'../images/pause1.png').convert_alpha()
    pause_pressed_image = pygame.image.load(r'../images/pause2.png').convert_alpha()
    pause_nor_image = pygame.transform.scale(pause_nor_image, (50, 50))
    pause_pressed_image = pygame.transform.scale(pause_pressed_image, (50, 50))
    pause_rect = pause_nor_image.get_rect()
    pause_rect.right = width - 30
    pause_rect.top = 10
    resume_nor_image = pygame.image.load(r'../images/resume1.png').convert_alpha()
    resume_pressed_image = pygame.image.load(r'../images/resume2.png').convert_alpha()
    resume_nor_image = pygame.transform.scale(resume_nor_image, (80, 50))
    resume_pressed_image = pygame.transform.scale(resume_pressed_image, (80, 50))
    resume_rect = resume_nor_image.get_rect()
    resume_rect.midtop = pause_rect.midtop
    pause_image = pause_nor_image
    # 暂停操作
    pause = False
    # 载入游戏开始画面
    play_nor_image = pygame.image.load(r'../images/play1.png').convert_alpha()
    play_pressed_image = pygame.image.load(r'../images/play2.png').convert_alpha()
    play_rect = play_nor_image.get_rect()
    play_rect.center = width // 2, height // 2 + 100
    play_image = play_nor_image
    # 载入yes
    yes_nor_image = pygame.image.load(r'../images/yes1.png').convert_alpha()
    yes_pressed_image = pygame.image.load(r'../images/yes2.png').convert_alpha()
    yes_rect = yes_nor_image.get_rect()
    yes_rect.center = width // 2 - 100, height // 2 + 150
    yes_image = yes_nor_image
    # 载入no
    no_nor_image = pygame.image.load(r'../images/no1.png').convert_alpha()
    no_pressed_image = pygame.image.load(r'../images/no2.png').convert_alpha()
    no_rect = no_nor_image.get_rect()
    no_rect.center = width // 2 + 100, height // 2 + 150
    no_image = no_nor_image
    # 开始状态
    play = False
    # 圣诞老人图片切换
    switch = 0
    # 礼物计时器
    PRESENTTIMER = 5 * frame_num
    # 面板载入
    board_image = pygame.image.load(r'../images/board.png').convert_alpha()
    board_image = pygame.transform.scale(board_image, (450, 300))
    board_rect = board_image.get_rect()
    board_rect.midtop = (250, 50)
    # 音乐播放控制
    music = False
    # 生命设置
    life_num = 3
    life_image = pygame.image.load(r'../images/圣诞老人1.png').convert_alpha()
    life_image = pygame.transform.scale(life_image, (40, 40))
    life_rect = life_image.get_rect()
    # 读入字体
    score_font = pygame.font.Font(r'../font/奇妙圣诞夜.ttf', 40)
    font1 = pygame.font.Font(r'../font/奇妙圣诞夜.ttf', 50)
    # 阅读时间限制
    timelimit = 0
    # 获胜状态
    win = False
    # 问题状态
    question = 0
    while True:
        if gameover:
            pygame.quit()
            sys.exit()
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == MOUSEMOTION:
                if not play:
                    # 开始按钮切换
                    if play_rect.collidepoint(event.pos):
                        play_image = play_pressed_image
                    else:
                        play_image = play_nor_image
                else:
                    if not win:
                        if life_num > 0:
                            # 暂停按钮切换
                            if not pause:
                                if pause_rect.collidepoint(event.pos):
                                    pause_image = pause_pressed_image
                                else:
                                    pause_image = pause_nor_image
                            else:
                                if pause_rect.collidepoint(event.pos):
                                    pause_image = resume_pressed_image
                                else:
                                    pause_image = resume_nor_image
                        else:
                            # yes,no 按钮切换
                            if yes_rect.collidepoint(event.pos):
                                yes_image = yes_pressed_image
                            else:
                                yes_image = yes_nor_image
                            if no_rect.collidepoint(event.pos):
                                no_image = no_pressed_image
                            else:
                                no_image = no_nor_image
                    else:
                        # yes,no 按钮切换
                        if yes_rect.collidepoint(event.pos):
                            yes_image = yes_pressed_image
                        else:
                            yes_image = yes_nor_image
                        if no_rect.collidepoint(event.pos):
                            no_image = no_pressed_image
                        else:
                            no_image = no_nor_image
            elif event.type == MOUSEBUTTONDOWN:
                if not play:
                    # 开始操作
                    if event.button == 1 and play_rect.collidepoint(event.pos):
                        play = True
                        # 播放背景音乐
                        pygame.mixer.music.play(-1)
                else:
                    if not win:
                        if life_num > 0:
                            # 暂停操作
                            if event.button == 1 and pause_rect.collidepoint(event.pos):
                                if pause == False:
                                    pause = True
                                    pause_image = resume_pressed_image
                                    pygame.mixer.music.pause()

                                else:
                                    pause = False
                                    pause_image = pause_pressed_image
                                    pygame.mixer.music.unpause()
                        else:
                            if event.button == 1 and yes_rect.collidepoint(event.pos):
                                life_num = 3
                            elif event.button == 1 and no_rect.collidepoint(event.pos):
                                gameover = True
                    else:
                        if question == 1:
                            if event.button == 1 and yes_rect.collidepoint(event.pos):
                                timelimit += 1
                            elif event.button == 1 and no_rect.collidepoint(event.pos):
                                gameover = True
                        if question == 2:
                            if event.button == 1 and yes_rect.collidepoint(event.pos):
                                getpresent = True
                                timelimit += 1
                            elif event.button == 1 and no_rect.collidepoint(event.pos):
                                getpresent = False
                                timelimit += 1

        screen.blit(bg_image, (0, 0))

        if not play:
            screen.blit(play_image, play_rect)
            screen.blit(board_image, board_rect)
            text1 = font1.render('MERRT  CHRISTMAS', True, (255, 255, 255))
            text2 = font1.render('TO MY BABY KY', True, (255, 255, 255))
            screen.blit(text1, (60, 120))
            screen.blit(text2, (100, 220))
        else:
            if not gameover:
                if life_num > 0:
                    if score > 10000:
                        win = True
                    if win:
                        if not music:
                            pygame.mixer.music.load(r'../sound/We Wish You A Merry Christmas.ogg')
                            pygame.mixer.music.set_volume(0.5)
                            pygame.mixer.music.play(-1)
                            music = True
                        screen.blit(board_image, board_rect)
                        timelimit += 1
                        if timelimit < 180:
                            text1 = font1.render('CONGRATULATIONS！', True, (255, 255, 255))
                            screen.blit(text1, (40, 150))
                        elif timelimit == 180:
                            question = 1
                            text1 = font1.render('DO YOU WANT', True, (255, 255, 255))
                            text2 = font1.render('A PRESENT ?', True, (255, 255, 255))
                            screen.blit(text1, (115, 120))
                            screen.blit(text2, (130, 200))
                            screen.blit(yes_image, yes_rect)
                            screen.blit(no_image, no_rect)
                            timelimit -= 1
                        elif timelimit > 180 and timelimit < 360:
                            text1 = font1.render('BUT YOU NEED TO', True, (255, 255, 255))
                            text2 = font1.render('ANSWER SANTA WU', True, (255, 255, 255))
                            text3 = font1.render('A QUESTION', True, (255, 255, 255))
                            screen.blit(text1, (65, 100))
                            screen.blit(text2, (60, 175))
                            screen.blit(text3, (130, 250))
                        elif timelimit == 360:
                            question = 2
                            text1 = font1.render('ARE YOU BOYFRIEND', True, (255, 255, 255))
                            text2 = font1.render('A HANDSOME BOY?', True, (255, 255, 255))
                            screen.blit(text1, (50, 120))
                            screen.blit(text2, (65, 200))
                            screen.blit(yes_image, yes_rect)
                            screen.blit(no_image, no_rect)
                            timelimit -= 1
                        else:
                            if getpresent:
                                text1 = font1.render('PASSWORD IS', True, (255, 255, 255))
                                text2 = font1.render('CKYISPIG1224', True, (255, 255, 255))
                                screen.blit(text1, (110, 120))
                                screen.blit(text2, (105, 200))
                                timelimit -= 1
                            else:
                                text1 = font1.render('PLAY AGAIN!! :  (', True, (255, 255, 255))
                                screen.blit(text1, (60, 150))
                                if timelimit > 540:
                                    win = False
                                    score = 0



                    else:
                        if music and not getpresent:
                            pygame.mixer.music.load(r'../sound/jingle bells.ogg')
                            pygame.mixer.music.set_volume(0.5)
                            pygame.mixer.music.play(-1)
                            music = False
                        if not pause:
                            # 礼物掉落
                            PRESENTTIMER -= 1
                            if PRESENTTIMER == 0:
                                mypresent.reset()
                                PRESENTTIMER = 5 * frame_num

                            # 炸弹碰撞检测
                            bomb_down = pygame.sprite.spritecollide(mysanta, mybomb, False, pygame.sprite.collide_mask)
                            if bomb_down:
                                for each in bomb_down:
                                    each.active = False
                                mysanta.active = False
                                life_num -= 1

                            # 得分检测
                            candy_down = pygame.sprite.spritecollide(mysanta, mycandy, False,
                                                                     pygame.sprite.collide_mask)
                            if candy_down:
                                for each in candy_down:
                                    each.active = False
                                    score += 100

                            if mypresent.active:
                                present_down = pygame.sprite.collide_mask(mysanta, mypresent)
                                if present_down:
                                    mypresent.active = False
                                    score += 500

                            # 绘制圣诞老人
                            if mysanta.active:
                                key_pressed = pygame.key.get_pressed()
                                # 获取键盘状态
                                if key_pressed[K_a] or key_pressed[K_LEFT]:
                                    mysanta.move_left()
                                if key_pressed[K_d] or key_pressed[K_RIGHT]:
                                    mysanta.move_right()

                                if delay % 5 == 0:
                                    switch += 1
                                if switch == 9:
                                    switch = 0

                            else:
                                mysanta.reset()
                                delay = 0
                                switch = 0
                            screen.blit(mysanta.image[switch], mysanta.rect)
                            # 延迟减少
                            delay -= 1
                            if delay == -99:
                                delay = 0

                            # 绘制生命
                            for each in range(1, life_num + 1):
                                screen.blit(life_image, (width - each * life_rect.width, height - life_rect.height))

                            # 绘制糖果
                            for each in mycandy:
                                screen.blit(each.image, each.rect)
                                if not each.active:
                                    each.reset()
                                else:
                                    each.move()
                            # 绘制礼物
                            if mypresent.active:
                                mypresent.move()
                                screen.blit(mypresent.image, mypresent.rect)

                            # 绘制炸弹
                            for each in mybomb:
                                screen.blit(each.image, each.rect)
                                if not each.active:
                                    each.reset()
                                else:
                                    each.move()

                        if not pause:
                            screen.blit(pause_image, pause_rect)
                        else:
                            screen.blit(pause_image, resume_rect)
                    score_text = score_font.render('Score:' + str(score), True, (255, 255, 255))
                    screen.blit(score_text, (10, 5))
                else:
                    screen.blit(board_image, board_rect)
                    text1 = font1.render('DO YOU NEED', True, (255, 255, 255))
                    text2 = font1.render('ANOTHER CHANCE ?', True, (255, 255, 255))
                    screen.blit(text1, (120, 120))
                    screen.blit(text2, (60, 220))
                    screen.blit(yes_image, yes_rect)
                    screen.blit(no_image, no_rect)
        # 下雪
        for each in mysnow:
            screen.blit(each.image, each.rect)
            each.move()
        pygame.display.flip()
        clock.tick(frame_num)


if __name__ == '__main__':
    try:
        main()
    except SystemExit:
        pass
    except:
        traceback.print_exc()
        pygame.quit()
        input()
