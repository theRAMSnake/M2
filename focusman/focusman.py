import threading
import time
import sys
import random
import pygame

stop_event = threading.Event()
pygame.init()
pygame.mixer.init()
pygame.mixer.music.load("./focusman/1.wav")

def do():
    random.seed()

    start_time = time.time()

    while not stop_event.is_set():
        time.sleep(0.99)
        if random.random() < 0.00055:
            print("A coin!")
            pygame.mixer.music.play(1)

    end_time = time.time()

    print("Time spent: {}".format(round((end_time - start_time) / 60)))

def main():
    calc_thread = threading.Thread(target=do)
    calc_thread.start()

    input("Working...\n")
    stop_event.set()

    calc_thread.join()

if __name__=="__main__":
    main()
