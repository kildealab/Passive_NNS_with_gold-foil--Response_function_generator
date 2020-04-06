import os
import fnmatch
import operator
import numpy as np
import csv
import matplotlib.pyplot as plt

#This sets the directory of the input csv files.
inputDir = '../build'

#ask the user how many responses need to be plotted
#num = int(raw_input('How many responses you want to plot?'))

#making a list of all capture input files in the given directory
inputCaptureFiles = []
for filename in os.listdir(inputDir):
	if fnmatch.fnmatch(filename, 'Captures*.txt'):
		inputCaptureFiles.append(filename)
inputCaptureFiles.sort() #sort the list alphabetically
num = len(inputCaptureFiles) # get the number of responses to be plotted



#making a list of all cellflux input files in the give directory
inputFluxFiles = []
for filename in os.listdir(inputDir):
	if fnmatch.fnmatch(filename, 'CellFlux*.txt'):
		inputFluxFiles.append(filename)
inputFluxFiles.sort()#sort the list alphabetically

#read the second column (captures per mass) from the capture files to a list
CapturePerMass = []
idx = 0
for Cfile in inputCaptureFiles:
	inCfilePath = inputDir+'/'+Cfile
	CapturePerMass.append([]) #creates a multidimentional array depending on the number of inputCaptureFiles
	with open (inCfilePath) as inCapture:
		CfileRows = csv.reader(inCapture, quoting = csv.QUOTE_NONNUMERIC)  # reads the data as float (otherwise reads as strings) 
		next(CfileRows) #skip the header row
		for row in CfileRows:
			CapturePerMass[idx].append(row[1]) #append the data in the column 1 into the list
		idx += 1


#read the second column (neutrons coming in per area) from the Fluence files to a list and store the mean neutron fluence into a list
CellFlux = []
for CFfile in inputFluxFiles:
	inCFfilePath = inputDir+'/'+CFfile
	tempListCF = []
	with open (inCFfilePath) as Flux:
		CFfileRows = csv.reader(Flux, quoting = csv.QUOTE_NONNUMERIC)  # reads the data as float (otherwise reads as strings) 
		next(CFfileRows)#skip the header row
		for row in CFfileRows:
			tempListCF.append(row[0]) #append the data in the column 1 into the Temperory list
	CellFlux.append((tempListCF)) #make a list of mean neutron fluence for each moderator configuration


#Calculate the response
Response = []

for i in range (0,num):
  #  ResponseCF.append(CapturePerMass[i]/CellFlux[i])
    numerator = CapturePerMass[i]
    denominator = CellFlux[i]
    temp = []
    for j in range (0,52):
        temp.append(numerator[j]/denominator[j])
    Response.append(temp)

#To output a csv file with the response functions in the format required for unfolding algorithm
ans = raw_input('Do you want to save the response functions as a CSV file (Y/N)?')
if (ans == 'y' or ans == 'Y'):
	work_dir = os.getcwd()
	if os.path.exists(work_dir+'/Response functions.csv'):
		os.remove(work_dir+'/Response functions.csv')
	with open ('Response functions.csv', mode='a') as responseFile: #create a file in the append mode
		responseFile.truncate()
		response_writer = csv.writer(responseFile, delimiter = ';')
		for ind in range (0,num):
			response_writer.writerow(Response[ind])



#Read the energy_bins.csv file to load 52 energy bins 
energy = []
with open('energy_bins.csv') as energyBins:
     energy_reader = csv.reader(energyBins, quoting = csv.QUOTE_NONNUMERIC)
     for row in energy_reader:
         energy.append(row) 

#Read the cross section.txt file to load the corresponding Au197 capture cross sections for 52 energy bins
cross_sections = []
with open('cross section.txt') as crossSections:
     cross_reader = csv.reader(crossSections, quoting = csv.QUOTE_NONNUMERIC)
     for row in cross_reader:
         cross_sections.append(row)

plt.figure(figsize=(20,10)) # plot size
#plt.style.use('fivethirtyeight') # plot style

#plt.subplot(211)
#plt.plot(energy, cross_sections,'-',label = '$\sigma(\gamma,n)$')  
#plt.plot(energy, Response[0],'-',label='Isotropic')
#plt.plot(energy, Response[1],'-',label='Normal')
#plt.plot(energy, Response[2],'-',label='Parallel')

#plt.plot(energy, cross_sections,'-',label = '$\sigma(\gamma,n)$')     
#font = {'fontname':'Serif'}
#plt.xscale('log')  # log values along x axis
#plt.yscale('log')
#plt.legend()   # include legend
#plt.xlabel("Energy (MeV)", fontsize=16,**font) # x axis label
#plt.ylabel("nCapture cross-section (barns)", fontsize=16,**font) # y axis label

#Plotting the data
for index in range (0,num):
	if (index == 0):
         #plt.plot(energy,ResponseCF[index],'-',label='Bare')
         x=5
	else:
         plt.plot(energy,Response[index],'-',label='Shell'+ str(index))


#plt.subplot(212)
#plt.plot(energy, Response[0],'-',label='Isotropic')
#plt.plot(energy, Response[1],'-',label='Normal')
#plt.plot(energy, Response[2],'-',label='Parallel')

#plt.subplot(212)
 
#plt.plot(energy, ResponseCF[0],'-',label='Isotropic')
#plt.plot(energy, ResponseCF[1],'-',label='Normal')
#plt.plot(energy, ResponseCF[2],'-',label='Parallel')

#Plot attributes
font = {'fontname':'Serif'}
plt.xscale('log')  # log values along x axis
#plt.yscale('log')
plt.legend()   # include legend
plt.xlabel("Incident Neutron energy (MeV)", fontsize=16,**font) # x axis label
plt.ylabel("Response ($cm^{2}$.$g^{-1}$)", fontsize=16,**font) # y axis label
#plt.title('Response functions of NNS-Gold system',fontsize=20,**font) # plot tittle
# ask the user if the plot to be saved
yes = raw_input('Do you want the plot to be saved (Y/N)?')
if (yes == 'y' or yes == 'Y'):
     plotfile = raw_input('What should be the filename?')
     plt.savefig(plotfile)
     print "The plot is saved as \""+ plotfile+".png\" in the working directory"

plt.show()

