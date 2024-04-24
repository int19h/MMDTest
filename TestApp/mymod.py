def say(s):
    print(s)

def loop_forever():
    flag = True
    while flag:
        pass

def raise_eventually(direct):
    if direct:
        raise_direct()
    else:
        raise_indirect()
    return direct

def raise_direct():
    raise Exception('spam', 'eggs')
    #raise MyError

def raise_indirect():
    return 1 / 0

class MyError(Exception):
    pass
