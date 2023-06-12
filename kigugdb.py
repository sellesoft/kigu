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
        return f"color({r}, {g}, {b}, {a})"
pp.add_printer("color", "^color$", color_printer)

class str8_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        if not self.val['str']:
            return "{empty}"
        c = self.val['count']
        if c == 0:
            return "\"\""
        s = str(self.val['str']).split('"')[1][:min(c,32)] 
        if c > 32:
            s += "..."
        return f"\"{s}\""
pp.add_printer("str8", "^str8$", str8_printer)

# class kigu_array_printer:
#     def __init__(self,val):
#         self.val = val
    
#     def to_string(self):
#         return f"a pointer"
# pp.add_printer("kigu_array", r"^.*\\*$", kigu_array_printer)

gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)