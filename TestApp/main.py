import atexit
atexit.register(lambda: input('Press Enter to close ...'))

import os
import sys
import ctypes
import threading
import time
import mymod
import natmod

class DictObj(object):
    x = 1
    def __init__(self, y = 2, z = u'foo'):
        self.y = y
        self.z = z
        self.self = self
    def frob(self, q):
        return DictObj.x + self.y + q
    @property
    def prop(self):
        return self.self
    @prop.setter
    def prop(self, other):
        self.self = other

class SlotsObj(object):
    __slots__ = ['y', 'z', 'self']
    x = 1
    def __init__(self, y = 2.0, z = b'foo'):
        self.y = y
        self.z = z
        self.self = self
    def frob(self, q):
        return SlotsObj.x + self.y + q
    @property
    def prop(self):
        return self.self
    @prop.setter
    def prop(self, other):
        self.self = other
    def __getitem__(self, slice):
        return slice



def another_thread():
    def inner():
        i = 0
        while i >= 0:
            time.sleep(5)
            i += 1
            print(i)
    inner()

a_thread = threading.Thread(target = another_thread)
a_thread.daemon = True
a_thread.start()
#time.sleep(100)



def locals_and_types():
    an_int = 123
    a_long = an_int ** 5
    a_float = an_int / 1.5
    a_complex = -4.2j
    a_bool = an_int < a_float
    another_bool = not a_bool
    an_ascii_str = b'foo\x40\x80\xFFbar'
    a_unicode_str = u'Foo\u0040\u0080\u00FFBar'
    a_bytearray = bytearray(an_ascii_str)

    an_ascii_str = b'foobar'
    a_unicode_str = u'FooBar'

    a_tuple = (an_int, a_float, a_bool)
    a_tuple += (a_tuple,)
    another_tuple = a_tuple + (an_ascii_str, a_unicode_str)
    a_tuple_with_a_long_string = (an_int, a_unicode_str * 1000, an_int)

    a_set = {an_ascii_str, a_tuple, a_unicode_str}

    a_list = [an_int, a_bytearray, a_set]
    a_list.append(a_list)

    a_dict = {an_int : a_float, a_tuple : a_bool, another_bool : a_tuple, a_unicode_str : a_list}
    a_list.append(a_dict)

    a_dictobj = DictObj(an_int)
    a_dictobj.frob(an_int)
    v = a_dictobj.prop
    a_dictobj.prop = v
    a_list.append(a_dictobj)

    a_slotsobj = SlotsObj(a_float, a_list)
    a_slotsobj.frob(a_float)
    v = a_slotsobj.prop
    a_slotsobj.prop = v
    a_slotsobj[...]
    a_dict[an_ascii_str] = a_slotsobj

    a_natobj = natmod.NatObj(an_int, a_float, a_bool, an_ascii_str, a_unicode_str, a_bytearray, a_tuple, a_set, a_list, a_dict, a_dictobj, a_slotsobj)
    v = a_natobj.prop
    a_natobj.prop = v
    a_list.append(a_natobj)
    same_tuple = a_natobj.frob(*a_tuple)

    print(an_int, a_float, a_bool, an_ascii_str, a_unicode_str, a_bytearray, a_tuple, a_set, a_list, a_dict, a_dictobj, a_slotsobj, a_natobj)

    a_very_long_list = list(range(12345))
    a_very_long_list = [a_very_long_list] + a_very_long_list
    a_tuple_with_a_very_long_list = (1, a_very_long_list, (1, 2, 3))

    global a_global_int, a_global_float, an_unassigned_global
    an_unassigned_global = ()
    a_global_int = an_int
    a_global_float = a_float
    pass

a_global_int = 123
a_global_float = 123.456
locals_and_types()
an_unassigned_global = ()



def outer(n):
    n += 1
    def inner(m):
        r = m + n
        f = lambda p: r - p
        f(n)
        return r
    inner(-n)
    n = -n
    return inner

outer(a_global_int)(a_global_float)


def python_calling_python():
    python_calling_native()

def python_calling_native():
    mymod.say('Python calling native')
    natmod.callback(python_called_from_native)

def python_called_from_native():
    mymod.say('Python called from native')
    a_natobj = natmod.NatObj()
    a_natobj.frob(a_natobj)

python_calling_python()



from ctypes import *
try:
    natmod_dll = windll.LoadLibrary("natmod.pyd")
except:
    natmod_dll = windll.LoadLibrary("natmod_d.pyd")
natmod_dll.frub.argtypes = [c_int, c_int]
natmod_dll.frub.restype = c_int
n = natmod_dll.frub(-1, 2)


for directly in (True, False):
    try:
        #input('Press Enter to raise ...')
        mymod.raise_eventually(directly)
        pass
    except:
        pass
    finally:
        pass


def python_called_from_dotnet():
    mymod.say('Python called from .NET')
    a_natobj = natmod.NatObj()
    a_natobj.frob(a_natobj)
    return a_natobj

def python_calling_dotnet():
    n = dotnet_callback(python_called_from_dotnet)
    return n
