import turtle
import random

turtle.speed('slowest')

def randomWalkb(length):
    steps = []
    x,y = 0,0
    walkx,walky = [x],[y]
    for i in range(length):

        new = random.randrange(0,150000)
        if new%4 == 0:
            x += 1
        elif new%4 == 2:
            y += 1
        elif new%4 == 1: 
            x += -1
        else :
            y += -1
        walkx.append(x)
        walky.append(y)
    return [walkx,walky]

walk = randomWalkb(250000)


for x, y in zip(*walk):
    #multiply by 10, since 1 pixel differences are hard to see
    turtle.goto(x*10,y*10)

turtle.exitonclick()