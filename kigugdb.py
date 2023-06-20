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

class kigu_array_printer:
    def __init__(self,val):
        self.val = val
    
    # def display_hint(self):
    #     return 'array'

    def to_string(self):
        me = f"(({self.val.type}){self.val.referenced_value().address})"
        print(me)
        count = int(gdb.parse_and_eval(f"array_count({me})"))
        out = ""
        for i in range(count):
            if i > 5:
                break
            out += str(gdb.parse_and_eval(f"{me}[{i}]"))
        return out
pp.add_printer("kigu_array", r"\w+Array|\w+_array", kigu_array_printer)

gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)