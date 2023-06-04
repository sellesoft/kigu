#
# Contains pretty printers for use in gdb
#

import gdb
pp = gdb.printing.RegexpCollectionPrettyPrinter("kigu")

# class color_printer:
#     def __init__(self, val):
#         self.val = val

#     def to_string(self):
#         r,g,b,a = self.val["r"], self.val["g"], self.val["b"], self.val["a"]
#         return f"color({r}, {g}, {b}, {a})"
# pp.add_printer("color", "^color$", color_printer)

gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)
