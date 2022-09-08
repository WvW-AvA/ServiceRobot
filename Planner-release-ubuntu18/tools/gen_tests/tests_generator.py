#!/usr/bin/env python

"tests_generator.py -- randomly generates the environment and tasks descriptions"

import random
import sys

## parameters
gDir = './'

gTestSize = 100

gLocMinSize = 3
gLocMaxSize = 10

gSmallObjMinSize = 5
gSmallObjMaxSize = 15

gMisInfoMaxSize = 5
gErrInfoMaxSize = 5

gTaskMinSize = 0
gTaskMaxSize = 3

gInfoMinSize = 0
gInfoMaxSize = 4

gConsMinSize = 0
gConsMaxSize = 3

gContainers = [
'washmachine',
'closet',
'cupboard',
'refrigerator',
'microwave']

gBigObjs = [
'human',
'plant',
'couch',
'chair',
'sofa',
'bed',
'table',
'workspace',
'worktable',
'teapoy',
'desk',
'television',
'airconditioner']

gSmallObjs = [
'book',
'can',
'remotecontrol',
'bottle',
'cup']

gColor = [
# the number of empty string determines the proportion of no-color cases
'', '', 
'white', 'red', 'green', 'yellow', 'blue', 'black']

gTasks = [
'give', 'puton', 'goto', 'putdown', 'pickup', 'open', 'close', 'putin', 'takeout']

gPredicates = [
'at', 'plate', 'opened', 'closed']

## the main part
class BigObj:
    def __init__(self, obj_id, loc_id):
        self.obj_id = obj_id
        self.sort = random.choice(gBigObjs + gContainers)
        if self.sort in gContainers:
            self.type = 'container'
            self.opened = random.choice((True, False))
        else:
            self.type = ''
            self.opened = False
        self.loc_id = loc_id

class SmallObj:
    def __init__(self, obj_id, loc_list):
        self.obj_id = obj_id
        self.sort = random.choice(gSmallObjs)
        self.color = random.choice(gColor)
        self.loc_id = random.choice(loc_list)
        self.info = ''
        self.mis = ''
        self.err_w = ''
        self.err_r = ''
    def genInfo(self, type, big_objs):
        self.info = '(sort {0} {1}) (size {0} small)'.format(self.obj_id, self.sort)
        if self.color != '':
            self.info += ' (color {0} {1})'.format(self.obj_id, self.color)
        # location is a little complicated
        if self.loc_id == 0:
            self.info += ' (plate %d)'%self.obj_id
        elif self.loc_id == -1:
            self.info += ' (hold %d)'%self.obj_id
        else:
            loc_str = ''
            loc_str_w = ''
            loc_id = self.loc_id
            if type == 'err':
                while loc_id == self.loc_id:
                    loc_id = random.choice(list(big_objs.keys()))
                
            if big_objs[self.loc_id].type == 'container':
                loc_str = '(inside %d %d)' % (self.obj_id, self.loc_id)
                loc_str_w = '(inside %d %d)' % (self.obj_id, loc_id)
            else:
                loc_str += '(at %d %d)' % (self.obj_id, self.loc_id)
                loc_str_w += '(at %d %d)' % (self.obj_id, loc_id)
            if type == 'mis':
                self.mis = loc_str
            elif type == 'err':
                self.err_w = loc_str_w
                self.err_r = loc_str
            else:
                self.info += ' ' + loc_str

class Task:
    def __init__(self, big_objs, small_objs):
        self.type = random.choice(gTasks)
        rand_again = not hasContainers(big_objs) and (self.type == 'open' or self.type == 'close' or self.type == 'putin' or self.type == 'takeout')
        while rand_again:
            self.type = random.choice(gTasks)
            rand_again = (self.type == 'open' or self.type == 'close' or self.type == 'putin' or self.type == 'takeout')
        if self.type == 'give':
            self.var0 = random.choice(small_objs)
        elif self.type == 'puton':
            self.var0 = random.choice(small_objs)
            self.var1 = random.choice(big_objs)
        elif self.type == 'goto':
            self.var0 = random.choice(big_objs)
        elif self.type == 'putdown':
            self.var0 = random.choice(small_objs)
        elif self.type == 'pickup':
            self.var0 = random.choice(small_objs)
        elif self.type == 'open':
            self.var0 = random.choice(big_objs)
            while self.var0.type != 'container':
                self.var0 = random.choice(big_objs)
        elif self.type == 'close':
            self.var0 = random.choice(big_objs)
            while self.var0.type != 'container':
                self.var0 = random.choice(big_objs)
        elif self.type == 'putin':
            self.var0 = random.choice(small_objs)
            self.var1 = random.choice(big_objs)
            while self.var1.type != 'container':
                self.var1 = random.choice(big_objs)
        elif self.type == 'takeout':
            self.var0 = random.choice(small_objs)
            self.var1 = random.choice(big_objs)
            while self.var1.type != 'container':
                self.var1 = random.choice(big_objs)
    def genTask(self):
        if self.type == 'give':
            self.task = '(:task (give Human X) (:cond %s))'%randSmallObj(self.var0, 'X')
        elif self.type == 'puton':
            self.task = '(:task (puton X Y) (:cond %s))'%(randSmallObj(self.var0, 'X') + ' ' + randBigObj(self.var1, 'Y'))
        elif self.type == 'goto':
            self.task = '(:task (goto X) (:cond %s))'%randBigObj(self.var0, 'X')
        elif self.type == 'putdown':
            self.task = '(:task (putdown X) (:cond %s))'%randSmallObj(self.var0, 'X')
        elif self.type == 'pickup':
            self.task = '(:task (pickup X) (:cond %s))'%randSmallObj(self.var0, 'X')
        elif self.type == 'open':
            self.task = '(:task (open X) (:cond %s))'%randBigObj(self.var0, 'X')
        elif self.type == 'close':
            self.task = '(:task (close X) (:cond %s))'%randBigObj(self.var0, 'X')
        elif self.type == 'putin':
            self.task = '(:task (putin X Y) (:cond %s))'%(randSmallObj(self.var0, 'X') + ' ' + randBigObj(self.var1, 'Y'))
        elif self.type == 'takeout':
            self.task = '(:task (takeout X Y) (:cond %s))'%(randSmallObj(self.var0, 'X') + ' ' + randBigObj(self.var1, 'Y'))

class Info:
    def __init__(self, big_objs, small_objs):
        self.type = random.choice(gPredicates)
        rand_again = not hasContainers(big_objs) and (self.type == 'opened' or self.type == 'closed')
        while rand_again:
            self.type = random.choice(gPredicates)
            rand_again = (self.type == 'opened' or self.type == 'closed')
        if self.type == 'at':
            self.var0 = random.choice(small_objs)
            self.var1 = random.choice(big_objs)
        elif self.type == 'plate':
            self.var0 = random.choice(small_objs)
        elif self.type == 'opened':
            self.var0 = random.choice(big_objs)
            while self.var0.type != 'container':
                self.var0 = random.choice(big_objs)
        elif self.type == 'closed':
            self.var0 = random.choice(big_objs)
            while self.var0.type != 'container':
                self.var0 = random.choice(big_objs)
    def genInfo(self):
        if self.type == 'at':
            self.info = '(:info (at X Y) (:cond %s))'%(randSmallObj(self.var0, 'X') + ' ' + randBigObj(self.var1, 'Y'))
        elif self.type == 'plate':
            self.info = '(:info (plate X) (:cond %s))'%randSmallObj(self.var0, 'X')
        elif self.type == 'opened':
            self.info = '(:info (opened X) (:cond %s))'%randBigObj(self.var0, 'X')
        elif self.type == 'closed':
            self.info = '(:info (closed X) (:cond %s))'%randBigObj(self.var0, 'X')
            
class Cons:
    def __init__(self, task, info):
        self.type = random.choice(('not', 'not not'))
        self.task = task
        self.info = info
    def genCons(self):
        cons = ''
        if self.task != None:
            self.task.genTask()
            cons = self.task.task
        elif self.info != None:
            self.info.genInfo()
            cons = self.info.info
        if self.type == 'not':
            self.cons = '(:cons_not %s)'%cons
        elif self.type == 'not not':
            self.cons = '(:cons_notnot %s)'%cons

def randSmallObj(small_obj, var):
    preds = []
    preds.append('(sort %s %s)'%(var, small_obj.sort))
    if small_obj.color != '':
        preds.append('(color %s %s)'%(var, small_obj.color))
    size = random.randint(1, len(preds))
    random.shuffle(preds)
    result = preds[0]
    for i in range(1, size):    
        result += ' ' + preds[i]
    return result
    
def randBigObj(big_obj, var):
    preds = []
    preds.append('(sort %s %s)'%(var, big_obj.sort))
    if big_obj.type != '':
        preds.append('(type %s %s)'%(var, big_obj.type))
    size = random.randint(1, len(preds))
    random.shuffle(preds)
    result = preds[0]
    for i in range(1, size):    
        result += ' ' + preds[i]
    return result

def hasContainers(big_objs):
    for o in big_objs:
        if o.sort in gContainers:
            return True
    return False

def generate(test_id):
    testXML = open(gDir + 'test.' + str(test_id) + '.xml', 'w')
    testXML.write('<?xml version="1.0" encoding="utf-8"?>\n')
    testXML.write('<test>\n')
    testXML.write('<env>\n')
    
    bigObjs = {}
    smallObjs = []
    
    info = []
    mis = []
    err = []
    # 1. generate environment descriptions
    objID = 1
    hasHuman = False
    emptyPlate = True
    emptyHand = True
   
    # 1.1. randomize locations
    locSize = random.randint(gLocMinSize, gLocMaxSize)
    locList = range(1, locSize + 1)
    for loc_id in locList:
        bigObj = BigObj(objID, loc_id)
        while hasHuman and bigObj.sort == 'human':
            bigObj = BigObj(objID, loc_id)
        if not hasHuman and bigObj.sort == 'human':
            hasHuman = True
        bigObjs[loc_id] = bigObj
        objID += 1
        
    # 1.2. randomize small objects
    smallObjSize = random.randint(gSmallObjMinSize, gSmallObjMaxSize)
    # location id 0 means in the plate of robot; -1 means in the hand of robot
    _loc_list = range(-1, locSize + 1) 
    for obj_id in range(objID, objID + smallObjSize + 1):
        smallObj = SmallObj(obj_id, _loc_list)
        while (not emptyPlate and smallObj.loc_id == 0) or (not emptyHand and smallObj.loc_id == -1):
            smallObj = SmallObj(obj_id, _loc_list)
        if emptyPlate and smallObj.loc_id == 0:
            emptyPlate = False
        if emptyHand and smallObj.loc_id == -1:
            emptyHand = False
        smallObjs.append(smallObj)
        
    # 1.3. pick out some info to generate the missing and error one
    random.shuffle(smallObjs)
    
    misSize = random.randint(1, gMisInfoMaxSize)
    mis = smallObjs[0:misSize]
    # output the mis sorted by 'obj_id'
    mis = sorted(mis, key = lambda o:o.obj_id)
    
    errSize = random.randint(1, gErrInfoMaxSize)
    err = smallObjs[misSize:misSize + errSize]
    # output the err sorted by 'obj_id'
    err = sorted(err, key = lambda o:o.obj_id)
    
    info = smallObjs[misSize + errSize:]
        
    # 1.4. output to the xml file
    info_str = ''
    # robot state
    if emptyPlate:
        info_str += '(plate 0) '
    if emptyHand:
        info_str += '(hold 0) '
    rob_loc = random.choice(locList)
    info_str += '(at 0 %d)\n'%rob_loc
    
    for o in bigObjs.values():
        info_str += '(sort {0} {1}) (size {0} big) (at {0} {2})'.format(o.obj_id, o.sort, o.loc_id)
        if o.type == 'container':
            info_str += ' (type {0} container)'.format(o.obj_id)
            if o.opened:
                info_str += ' (opened {0})'.format(o.obj_id)
            else:
                info_str += ' (closed {0})'.format(o.obj_id)
        info_str += '\n'

    sortedInfoOjbs = []
        
    for o in info:
        o.genInfo('info', bigObjs)
        sortedInfoOjbs.append(o)
        
    mis_str = ''
    for o in mis:
        o.genInfo('mis', bigObjs)
        sortedInfoOjbs.append(o)
        if o.mis != '':
            mis_str += o.mis + '\n'
    
    err_str_w = ''
    err_str_r = ''
    for o in err:
        o.genInfo('err', bigObjs)
        sortedInfoOjbs.append(o)
        if o.err_w != '':
            err_str_w += o.err_w + '\n'
            err_str_r += o.err_r + '\n'
    
    # output the info sorted by 'obj_id'
    sortedInfoOjbs = sorted(sortedInfoOjbs, key = lambda o:o.obj_id)
    for o in sortedInfoOjbs:
        info_str += o.info + '\n'
    
    testXML.write('<info>\n%s</info>\n' % info_str)
    testXML.write('<mis>\n%s</mis>\n' % mis_str)
    testXML.write('<err>\n<r>\n%s</r>\n<w>\n%s</w>\n</err>\n' % (err_str_r, err_str_w))
    testXML.write('</env>\n')
    
    # 2. generate task description
    _big_objs = []
    for i in bigObjs.values():
        _big_objs.append(i)
    instr = '(:ins \n'
    taskSize = random.randint(gTaskMinSize, gTaskMaxSize)
    for i in range(0, taskSize):
        task = Task(_big_objs, smallObjs)
        task.genTask()
        instr += task.task + '\n'
    
    infoSize = random.randint(gInfoMinSize, gInfoMaxSize)
    for i in range(0, infoSize):
        info = Info(_big_objs, smallObjs)
        info.genInfo()
        instr += info.info + '\n'
        
    consSize = random.randint(gConsMinSize, gConsMaxSize)
    for i in range(0, consSize):
        cons = Cons(None, None)
        if cons.type == 'not not':
            cons.info = Info(_big_objs, smallObjs)
        elif random.choice((True, False)):
            cons.task = Task(_big_objs, smallObjs)
        else:
            cons.info = Info(_big_objs, smallObjs)
        cons.genCons()
        instr += cons.cons + '\n'
    
    instr += ')\n'
    testXML.write('<instr>%s</instr>\n' % instr)
    
    testXML.write('<nl>\n</nl>\n')
    testXML.write('</test>\n')

if __name__ == '__main__':
    if len(sys.argv) > 1:
        gDir = sys.argv[1] + '/'
    if len(sys.argv) > 2:
        gTestSize = int(sys.argv[2])
    testlist = open(gDir + 'test.list', 'w')
    for i in range(1, gTestSize + 1):
        testlist.write('test.' + str(i) + '.xml\n')
        generate(i)
