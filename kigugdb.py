#
# Contains pretty printers for use in gdb
#

import gdb
pp = gdb.printing.RegexpCollectionPrettyPrinter("kigu")

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

    def to_string(self):
        if not self.val['str'] or not self.val['count']:
            return "{empty}"
        c = self.val['count']
        s = str(self.val['str']).split('"')[1] 
        if s is None:
            return "{error}"
        return f"\"{s}\""
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


        
