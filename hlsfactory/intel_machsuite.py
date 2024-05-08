import os
import shutil
from pathlib import Path


# class AnnotateMachSuiteIntel:
#     def Annotate(parent_dir):
#         parent_dir = str(parent_dir) + "/machsuite_intel"

#         K = "common"
#         # folders = [f for f in os.listdir(parent_dir) if os.path.isdir(f)]
#         folders = [
#             f
#             for f in os.listdir(parent_dir)
#             if os.path.isdir(os.path.join(parent_dir, f))
#         ]

#         #  print ("path is ", parent_dir)
#         print("Folder names are", folders)
#         while K in folders:
#             folders.remove(K)

#         def copy_files_with_extension(source_folder, target_folder, extension):
#             for filename in os.listdir(source_folder):
#                 source_file = os.path.join(source_folder, filename)
#                 if os.path.isfile(source_file) and filename.endswith(extension):
#                     shutil.copy(source_file, target_folder)

#         for i in range(len(folders)):
#             #  print ("\nnames", folders[i])
#             root_path = os.path.join(parent_dir, folders[i])
#             #  print ("rootdir", root_path)
#             dest_path = os.path.join(root_path, "intel_src")
#             #  print ("dest dir", dest_path)
#             if not os.path.exists(dest_path):
#                 os.makedirs(os.path.join(root_path, "intel_src"))

#             copy_files_with_extension(root_path, dest_path, ".c")
#             copy_files_with_extension(root_path, dest_path, ".h")

#             # Find the name of the hls template file
#             hls_template = [
#                 filename
#                 for filename in os.listdir(root_path)
#                 if filename.startswith("hls_template")
#             ]
#             # Pick the module name from the template file
#             # print ("prefixed", hls_template[0])

#             # Pick the module name from the template file
#             fp = open(os.path.join(root_path, hls_template[0]))
#             lines = fp.readlines()
#             words = lines[1].split()
#             component_name = words[1]
#             # print ("component",component_name)

#             words = lines[2].split()
#             Cfile_name = words[1]
#             # Cfile_names = words[1].split('/')
#             # Cfile_name= Cfile_names[-1]

#             words = lines[3].split()
#             headerfile_name = words[1]

#             # print ("header name",headerfile_name)
#             fp.close()

#             # open the destination C file in intel_src and find the line number
#             with open(os.path.join(dest_path, Cfile_name)) as f:
#                 component_name = "void " + component_name
#                 for num, line in enumerate(f, 1):
#                     if component_name in line:
#                         words = line.split()
#                         # print ('dest file line',words)
#                         # print ('dest file line num',num)
#                         linenum = num
#             # print ('dest file line num now is',linenum)
#             f.close()

#             # append component word to the destination file
#             with open(os.path.join(dest_path, Cfile_name)) as f:
#                 lines = f.readlines()
#             #  print ('num is ', linenum)
#             f.close()

#             lines[linenum - 1] = "component " + lines[linenum - 1]

#             # write the edited content back to the file
#             with open(os.path.join(dest_path, Cfile_name), "w") as txtfile:
#                 txtfile.writelines(lines)

#             txtfile.close()

#             #####################################################################

#             # open the destination header file in intel_src and find the line numbers for #include and just replace those lines with new lines

#             f = open(os.path.join(dest_path, headerfile_name), "r")
#             lines = f.readlines()
#             new_lines = []
#             for line in lines:
#                 if "#include" not in line.strip():
#                     new_lines.append(line)

#             f.close()

#             header_text = (
#                 '\n\n#include <stdlib.h> \n #include <inttypes.h> \n#include <string.h>\n#include "../common/support.h" '
#                 + "\n"
#                 + "#include <HLS/hls.h> \n#include <HLS/stdio.h>\n#include <fcntl.h>\n#include <stdint.h>\n"
#             )

#             new_lines[4] = header_text + new_lines[4]
#             # write the edited content back to the file
#             with open(os.path.join(dest_path, headerfile_name), "w") as txtfile:
#                 txtfile.writelines(new_lines)

#             txtfile.close()
