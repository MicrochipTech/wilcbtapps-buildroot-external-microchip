import sys


FILEPATH = '../'+sys.argv[1]+'/Makefile'
FILEPATH_TEMP = 'Makefile_transparant_wifiprov' 


STRING_REF = "tools/btgatt-server$(EXEEXT): $(tools_btgatt_server_OBJECTS) $(tools_btgatt_server_DEPENDENCIES) $(EXTRA_tools_btgatt_server_DEPENDENCIES) tools/$(am__dirstamp)"


def update_file():
    try:
        with open(FILEPATH,'r') as hfile:
            content = hfile.readlines()
	    print content
            hfile.close()
    except Exception as e:
        print e
    i=1    

    for line in content:
        if STRING_REF in line:
            #print "Found @ Line#"+str(i)
            i+=4
            break
        else:
            i+=1

    print content[i] 
    try:
        with open(FILEPATH_TEMP,'r') as tfile:
            #data = tfile.readlines()
            data = tfile.read()
            tfile.close()
    except Exception as e:
        print e

    content[i]=data	 
    try:
        with open(FILEPATH,'w') as headerFile:
            headerFile.writelines(content)
            headerFile.close()
    except Exception as e:
        print e


if __name__ == '__main__':
    update_file()

