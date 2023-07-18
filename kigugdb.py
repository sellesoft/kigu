#
# Contains pretty printers for use in gdb
#

import gdb
pp = gdb.printing.RegexpCollectionPrettyPrinter("kigu")
import time

def dbgmsg(s):
    t = time.perf_counter()
    gdb.write(f"dbg:{t}: {s}"); gdb.flush()


class color_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        r,g,b,a = self.val["r"], self.val["g"], self.val["b"], self.val["a"]
        return f"({r}, {g}, {b}, {a})"
pp.add_printer("color", "^color$", color_printer)

class str8_printer:
    def __init__(self, val):
        self.val = val

    # TODO(sushi) this needs to be changed to directly read the string's 'str' value'
    #             in memory so that we only extract 'count' amount of memory
    #             so when we have a 10k line file with a str8 pointing into it,
    #             we dont read the entire file 
    def to_string(self):
        try:
            if not self.val['str'] or not self.val['count']:
                return "{empty}"
            c = self.val['count']
            s = str(self.val['str'])
            s = s[s.find('"'):]
            if s is None:
                return "{error}"
            i = 0
            while i < c:
                if s[i] == '\\':
                    c += 1
                i += 1
            return f"\"{s[1:c+1]}\""
        except Exception as e:
            print(f"{self.__class__.__name__} error: {e}")
pp.add_printer("str8", "^str8$", str8_printer)

class dstr8_printer:
    def __init__(self, val):
        self.val = val
    
    def to_string(self):
        return f"{self.val['fin']}"
pp.add_printer("dstr8", "^dstr8$", dstr8_printer)

class print_kigu_array(gdb.Command):
    def __init__(self):
        super(print_kigu_array, self).__init__("pka", gdb.COMMAND_USER, gdb.COMPLETE_COMMAND)
    
    def invoke(self, arg, tty):
        val = gdb.parse_and_eval(arg)
        if val == None:
            print("invalid expression given")
            return
        count = gdb.parse_and_eval(f"array_count({val})")
        if not count:
            print("{}")
            return
        gdb.execute(f"p *{arg}@{count}")
print_kigu_array()

gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)


        
