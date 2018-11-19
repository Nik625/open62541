import os
import csv
import re
from os.path import dirname

''' This function extracts the appropriate .csv-file to lists for the relevant header'''
def extract_csv_to_lists(filepath):
    with open(filepath, 'r') as z:
        reader  = csv.reader(z)
        tList1 = []
        tList2 = []
        tList3 = []
        tListC = []
        itereader = iter(reader)
        next(itereader) #skip first element of list (headlines)
        for row in itereader:
            tList1.append(row[1])
            tListC.append(row[0])
            tList2.append(row[3])
        tList2 = [x.replace(' ','_') for x in tList2] # replace whitespaces with underscores in list
        seen = set()
        dups = [idx for idx,item in enumerate(tList2) if item in seen or seen.add(item)] #get list of indexes where value has duplicates (kilometre)
        tList2[dups[0]-1] += '_KTM' #append unique id to applying element
        tList2[dups[0]] += '_KMT' 
        for elem in tList2:
            tList3.append("UA_DA_UNITID_" + elem.upper())
        tStr = ' '.join(tList3) # convert the list to a string
        tStr.replace(' ','\n') # replace whitespaces with newlines
        new_tStr = re.sub('[^ a-zA-Z0-9_]', '', tStr) # filter all special characters except underscores
        tList3 = new_tStr.split() # reconvert string to list with comma separators
        z.close()
        return tList1, tList3

''' This function extracts the appropriate .csv-file to lists containing also a name attribute for the relevant header
    For further details see the upper function'''
def get_EnigneeringUnit_name(filepath):
    with open(filepath, 'r') as z:
        reader  = csv.reader(z)
        tList2 = []
        tList3 = []
        itereader = iter(reader)
        next(itereader)
        for row in itereader:
            tList2.append(row[3])
        tList2 = [x.replace(' ','_') for x in tList2]
        seen = set()
        dups = [idx for idx,item in enumerate(tList2) if item in seen or seen.add(item)]
        tList2[dups[0]-1] += '_KTM'
        tList2[dups[0]] += '_KMT' 
        for elem in tList2:
            tList3.append("UA_DA_UNITID_" + elem.upper() +"_NAME")
        tStr = ' '.join(tList3)
        tStr.replace(' ','\n')
        new_tStr = re.sub('[^ a-zA-Z0-9_]', '', tStr)
        tList3 = new_tStr.split()
        return tList2, tList3

def generate_DA_UNITID_NAME_header(fileoutput, filesource):
    tList1 = []
    tList2 = []
    tList1, tList2 = get_EnigneeringUnit_name(filesource)
    cstring = "/* Unit ID Name Definitions */"
    with open(fileoutput,'r') as f:
        data = f.readlines()
        data.append("\n"+"\n"+"\n"+cstring+"\n"+"\n"+"\n"+"\n")
        #f.write("\n"+"\n"+"\n"+cstring+"\n"+"\n"+"\n"+"\n")
        padding = len(max(tList2, key=len)) #longest element used for minimal distance of value
        s = ' '
        for idx, elem in enumerate(tList2):
            tstring = ("#define "+tList2[idx]+ ((padding-len(elem))+3)*s + "\""+ tList1[idx]+"\"\n")
            data.append(tstring)
            #f.write(tstring)
        data.append("\n"+"#endif")
        data = "".join(data)
    with open(fileoutput,'w') as f:
        f.write(data)
        #f.write("\n"+"#endif")
    f.close()


def generate_DA_UNITID_header(fileoutput, filesource):
    tList1 = []
    tList2 = []
    tList1, tList2 = extract_csv_to_lists(filesource)
    cstring = ("/**************************************************************************************"+"\n"+
            "Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved."+"\n"+
            "***************************************************************************************"+"\n"
            "$Id:  $:"+"\n"+"\n"+
            "Description:"+"\n"+
            "netPROXY Package OPC UA Server - Open62541 DataAccess UNIT_ID Generation of Header"+"\n"+
            "*/"+"\n"+"\n"+"\n"+
            "/*****************************************************************************/"+"\n"
            "/* UNIT_ID DA DEFINITIONS                                                    */"+"\n"+
            "/*****************************************************************************/"+"\n"+"\n"+"\n"+"\n"+
            "#ifdef UA_ENABLE_DA"+"\n"+"\n")
    with open(fileoutput,'w') as f:
        f.write(cstring)
        padding = len(max(tList2, key=len))
        s = ' '
        for idx, elem in enumerate(tList2):
            tstring = ("#define "+tList2[idx]+ ((padding-len(elem))+3)*s + tList1[idx] + "\n")
            f.writelines(tstring)
    f.close()



# def generate_include(file_path_name):
#     with open(file_path_name, "r") as f:
#         data = f.readlines()
#     data.insert(30,'\n'+'\n'+'/* DataAccess UnitIDs from OPC Specification Part 8 */'+\
#                 '\n'+'#include "NpxPck_OpcUaServer_DataAccess_UnitID.h"'+'\n'+\
#                 '/* No more includes */'+'\n'+'\n')
#     with open(file_path_name, "w+") as f:
#         data = "".join(data)
#         f.write(data)
#         #f.writelines(data)


if __name__ == "__main__":

    project_dir = (os.getcwd()).split("OpcUaLib")[0] #split at OpcUaLib
    print(project_dir)
    npx_dir = project_dir + "OpcUaLib\Sources\open62541_lib\include\ua_server_dataaccess.h"

    generate_DA_UNITID_header(npx_dir, project_dir + r"OpcUaLib\Sources\open62541_lib\tools\schema\UNECE_to_OPCUA.csv")
    generate_DA_UNITID_NAME_header(npx_dir, project_dir + r"OpcUaLib\Sources\open62541_lib\tools\schema\UNECE_to_OPCUA.csv")
    '''generate respective includes for source file'''
    # project_dir = os.getcwd().split("OpcUaLib")[0]
    # generate_include(project_dir + r"netPROXY\Packages\OpcUaServer\Core\Sources\NpxPck_OpcUaServer_DataAccess.c")
