###############################################
# Convert .param files into graph.xml files.
###############################################

import glob
import os

types = {   "1" : "temporal",
            "2" : "histogram",
            "4" : "synoptic",
            "4" : "synopticv",
            "8" : "synoptich",
           "16" : "graph_dataarray",
           "32" : "eventcount",
           "64" : "graph",
          "128" : "graph2d",
          "256" : "histo2d",
          "512" : "histo3d",
         "1024" : "synoptic_single_value",
         "1025" : "temporal, synoptic_single_value",
         "1037" : "temporal, synopticv, synoptich, synoptic_single_value",
         "2048" : "grapherror_dataarray",
         "4096" : "histo_dataarray" }


filenames = glob.glob("*.param")
filenames+= glob.glob("*.ql_file2")
filenames+= glob.glob("*.ql_file2GPS")
filenames+= glob.glob("*.ql_file2PPS")
for filename in filenames:
    fileout = open(os.path.splitext(filename)[0]+"_graph.xml" , 'w')

    fileout.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
    fileout.write("<graphs>\n")

    file = open(filename, 'r')
    fileout.write("<!--####################################################################-->\n")
    fileout.write("<!-- File: " + filename+" -->\n")

    for line in file:
        # removing whitespaces
        line = line.strip()

        if len(line) == 0:
            continue

        # split into fields
        fields = [x.strip() for x in line.split('|')]

        # save graph
        type = types[fields[3]]
        strout = "<graph id=\""+fields[0]+"\" type=\""+type+"\" name=\""+fields[2]+"\""
        if fields[5] != "":
            strout+=" title=\""+fields[5]+"\""

        if fields[6] != "na":
            strout+=" temporalScale=\""+fields[6]+"\""

        if fields[9] != "na":
            strout+=" temporalBinSize=\""+fields[9]+"\""

        if fields[12] != "na":
            strout+=" yMinLimit=\""+fields[12]+"\""

        if fields[13] != "na":
            strout+=" yMaxLimit=\""+fields[13]+"\""

        if fields[14] != "na":
            strout+=" minYhist=\""+fields[14]+"\""

        if fields[15] != "na":
            strout+=" maxYhist=\""+fields[15]+"\""

        if fields[16] != "na":
            strout+=" temporalWindow=\""+fields[16]+"\""

        if fields[18] != "none":
            strout+=" extra=\""+fields[18]+"\""

        if fields[19] != "":
            strout+=" lookupTable=\""+fields[19]+"\""

        if len(fields)>20 and fields[20] != "none":
            strout+=" calcurve=\""+fields[20]+"\""

        if len(fields)>21 and fields[21] != "none":
            strout+=" validRange=\""+fields[21]+"\""

        strout+=">\n"

        # save axis
        refs = [x.strip() for x in fields[1].split(',')]
        labels = [x.strip() for x in fields[7].split(',')]
        nbins = [x.strip() for x in fields[8].split(',')]
        mins = [x.strip() for x in fields[10].split(',')]
        maxs = [x.strip() for x in fields[11].split(',')]
        errorlabels = []
        if len(fields)>22:
            errorlabels = [x.strip() for x in fields[22].split(',')]

        if len(refs) > 0:
            strout+="    <xaxis ref=\""+refs[0]+"\""
            if len(labels) >= 1:
                strout+=" label=\""+labels[0].strip()+"\""
            if nbins[0] != "na":
                strout+=" nbins=\""+nbins[0]+"\""
            if mins[0] != "na":
                strout+=" min=\""+mins[0]+"\""
            if maxs[0] != "na":
                strout+=" max=\""+maxs[0]+"\""
            if len(errorlabels) >= 1 and errorlabels[0] != "none":
                strout+=" errorLabel=\""+errorlabels[0]+"\""
            strout+=" />\n"
        if len(refs) > 1:
            strout+="    <yaxis ref=\""+refs[1]+"\""
            if len(labels) >= 2:
                strout+=" label=\""+labels[1].strip()+"\""
            if len(nbins) >= 2 and nbins[1] != "na":
                strout+=" nbins=\""+nbins[1]+"\""
            if len(mins) >= 2 and mins[1] != "na":
                strout+=" min=\""+mins[1]+"\""
            if len(maxs) >= 2 and maxs[1] != "na":
                strout+=" max=\""+maxs[1]+"\""
            if len(errorlabels) >= 2 and errorlabels[1] != "none":
                strout+=" errorLabel=\""+errorlabels[1]+"\""
            strout+=" />\n"
        if len(refs) > 2:
            strout+="    <zaxis ref=\""+refs[2]+"\""
            if len(labels) >=3:
                strout+=" label=\""+labels[2].strip()+"\""
            if len(nbins) >= 3 and nbins[2] != "na":
                strout+=" nbins=\""+nbins[2]+"\""
            if len(mins) >= 3 and mins[2] != "na":
                strout+=" min=\""+mins[2]+"\""
            if len(maxs) >= 3 and mins[2] != "na":
                strout+=" max=\""+maxs[2]+"\""
            if len(errorlabels) >= 3 and errorlabels[2] != "none":
                strout+=" errorLabel=\""+errorlabels[2]+"\""
            strout+=" />\n"
        strout+="</graph>\n"
        fileout.write(strout)

    fileout.write("</graphs>\n")
    fileout.close()
