#!/usr/bin/env python

"run this python to generate the tests and validate the tests"

import platform
import os
import subprocess
import time

# parameters
gTestSize = 10

_projRootPath = ''

def copyExec():
    if platform.system() == 'Windows':
        os.system('copy {0}\\bin\\Release\\validator.exe .'.format(_projRootPath))
        os.system('copy {0}\\bin\\Release\\cserver.exe .'.format(_projRootPath))
        os.system('copy {0}\\lib\\Release\\asp.dll .'.format(_projRootPath))
        os.system('copy {0}\\res\\*.lp .'.format(_projRootPath))
        os.system('copy {0}\\res\\*.bat .'.format(_projRootPath))
        os.system('copy {0}\\res\\iclingo.win iclingo.exe'.format(_projRootPath))
    elif platform.system() == 'Linux':
        os.system('cp {0}/bin/validator .'.format(_projRootPath))
        os.system('cp {0}/bin/cserver .'.format(_projRootPath))
        os.system('cp {0}/bin/vrunact.sh .'.format(_projRootPath))
        os.system('cp {0}/bin/vruntask.sh .'.format(_projRootPath))
        os.system('cp {0}/lib/libasp.so .'.format(_projRootPath))
        os.system('cp {0}/res/*.lp .'.format(_projRootPath))
        os.system('cp {0}/res/iclingo .'.format(_projRootPath))
    else:
        print('Cannot support other platform except Windows and Linux.')

def run():
    if platform.system() == 'Windows':
        cserver = subprocess.Popen('cserver.exe -td _tmp -eval asp')
        time.sleep(0.5)
        validator = subprocess.Popen('validator.exe')
        validator.wait()
        cserver.kill()
    elif platform.system() == 'Linux':
        cserver = subprocess.Popen('./cserver -td _tmp -eval ./libasp', shell=True)
        time.sleep(0.5)
        validator = subprocess.Popen('./validator', shell=True)
        validator.wait()
        cserver.kill()
        os.system("kill -9 $(ps ax|grep 'cserver'|grep -v 'grep'|awk '{print $1}') 1>/dev/null 2>/dev/null")
    else:
        print('Cannot support other platform except Windows and Linux.')

def copyTests():
    if platform.system() == 'Windows':
        os.system('mkdir tests')
        os.system('copy test.list tests')
        testlist = open('test.list')
        testnames = testlist.readlines()
        count = 0
        for name in testnames:
            count += 1
            name = name.rstrip('\n\r')
            os.system('copy _tmp\%s tests'%name)
        testlist.close()
        os.system('del /Q _tmp test.list')
        os.system('rd _tmp')
        print("===> Generated %d tests."%count)
    elif platform.system() == 'Linux':
        os.system('mkdir tests')
        os.system('cp test.list tests')
        testlist = open('test.list')
        testnames = testlist.readlines()
        count = 0
        for name in testnames:
            count += 1
            name = name.rstrip('\n\r')
            os.system('cp _tmp/%s tests'%name)
        testlist.close()
        os.system('rm -r _tmp/ test.list')
        print("===> Generated %d tests."%count)
    else:
        print('Cannot support other platform except Windows and Linux.')
    
        
if __name__ == '__main__':
    if platform.system() == 'Windows':
        _projRootPath = '..\\..'
    elif platform.system() == 'Linux':
        _projRootPath = '../..'
    else:
        print('Cannot support other platform except Windows and Linux.')
        sys.exit(0)
    os.system('mkdir _tmp')
    os.system('python tests_generator.py _tmp %d'%gTestSize)
    copyExec()
    run()
    copyTests()
    
