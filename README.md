# Passive_NNS_with_gold-foil---Response_functions_generator
This is a Monte-Carlo simulation application based on GEANT4 [1] to generate the system response functions of a passive Nested Neutron Spectrometer (NNS) [2]. Passive NNS consists of seven cylindrical HDPE moderator shells and a gold-foil as a thermal neutron detector. The gold-foil is positioned at the geometrical centre of the moderators in a horizontal disposition using two cylindrical inserts. This spectrometer can be used to measure the neutron flux spectra but the spectrometer response functions are essential to unfold the neutron fulx spectra from the measurements [3,4]. This application will enable the passive NNS user to generate the system response functions. 

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3742354.svg)](https://doi.org/10.5281/zenodo.3742354)

## Table of Contents

* [Author](#author)
* [Prerequisites](#Prerequisites)
* [Features](#Features)
* [Program structure](#Program-structure)
* [Getting started](#Getting-started)
* [Author's notes](#Author's-notes)
* [License](#license)
* [References](#References)

## Author
Felix Mathew\
Contact email: felix.mathew@mail.mcgill.ca

## Prerequisites
- Geant4.10.04.p02 ( See [the installation guide](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/InstallationGuide/BackupVersions/V10.4/html/index.html) to install Geant4)


## Features:
* User can choose the moderator configuration to be used in the simulation
* QGSP_BIC_HP high-precision neutron physics model
* Included the thermal neutron elastic scattering data below 4 eV.
* Simulates 10<sup>7</sup> mono-energetic neutron sources in each run
* Multithreaded to enable parallel runs
* Importance sampling variance reduction technique included
* User can switch between two scorers (neutron capture scorer and neutron cellflux scorer)
* Individual simulation involve 52 mono-energetic runs spanning 1 meV to 16 MeV

## Program structure
More about the basic structure and the functions used can be found in Geant4: [Book for application developers](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/GettingStarted/gettingStarted.html) 

### Detector construction
The passive NNS is modeled using the Detector Construction class. Spectrometer's geometry, dimentions and materials are specified here.
We used a gold-foil with 99% purity and a thickness of 0.1 mm to construct the passive NNS. 
### Particle source
The General Particle Source (GPS) is used to generate neutron sources. GPS is enabled in the Primary Generator Action class and evoked using macro files.
### Physics model
The QGSP_BIC_HP high-precision neutron physics model is used for neutron transport in most materials. This model is modified to include the thermal neutron elastic scattering data below 4 eV for materials available in the G4NeutronHPThermalScattering class of GEANT4. These materials are specified with their required name settings in the Detector Construction class.  
### Variance reduction techniques
The importance sampling technique that encorporate both geometric splitting and Russian roulette techniques is used in the simulation. The importance of different regions of the model is specified using the Importance Detector Construction class as required. 
### Scoring and output
The region corresponding to the gold-foil is assigned as the sensitive detector region. The sensitive detector can score either the number of neutron capture interactions in the region or the neutron cellflux in the region. Two scorers are used nCapture scorer and nCellFlux scorer respectively. Using the Run Action class, the scores (number of neutron capture per unit mass of the gold foi and the neutron cellflux) are output as CSV files. One files is created per the moderator configuration and the scorer used. See author's note to read different arguments used to run the program and to choose the scorers. 

## Getting started
1. Download this repository
2. Make seperate directories called 'build' and 'install' inside the main repository.
   <pre><code>mkdir build install && cd build</code></pre>
3. Build the program in the build directory.
   <pre><code>cmake -DCMAKE_INSTALL_PREFIX=../install -DGEANT$_DIR=$G4CMAKEDIR ../</code></pre>
4. Make and install the program in the build directory
   <pre><code>make && make install</code></pre>
5. Run the program for the shell configuration of your interest (See authors notes for more information)
   <pre><code>./NNSResponse -m shell2.mac -shell 2 -scorer 0</code></pre>

## Author's notes
* It is important to note that, any change in the foil or foil configuration results in a completely different response function and the sufficient modifications should be made according to the experimental conditions.
* The simulation must be performed separately to get the number of capture interactions on the foil and the neutron fluence on the foil. This is because the neutron capture is scored with the gold-foil whereas the neutron flux required is in the absense of the spectrometer materials.
* The program takes two necessary arguments and one optional argument -m, -shell and -scorer respectively. The arguments -m requires the name of the macrofile to define the neutron source. Argument -shell takes in the number of moderators to be used in the simulation. The user can choose from 0 to 7, where zero corresponds to the configuration with no moderator (only bottom insert) and seven corresponds to the configuration with all seven moderators. The optional argument -scorer can take either 0 or 1. This determines the scorer used in the simulation. It is by default set to zero (the neutron capture scorer) and the user can change it to 1 to get the neutron cell flux score. 
* The macro files are scripted to build a cylindrical particle source suface with dimensions bigger than the biggest moderator in the configuration. The neutron sources are set to get distributed randomly on this surface and generate neutrons in random direaction from the source surface to ensure complete field isotropy. User must ensure the macro file number corresponds to the shell configuration in the simulation. (i.e when the argument -shell is using '4' as an input the -m argument must use 'shell4.mac' file as input when scoring neutron capture)
* The neutron fluence (cellflux) should be scored without any moderator shell and in air. Hence, when scoring for cell flux, the -shell argument will be set to zero by default. 
* We defines the response of the system as the ratio of neutron capture per unit mass to the cell flux. A program ("Plotting script") written in python can be used to generate a CSV and a PNG files for the response functions. The program takes in the output files from the simulation from their directories and produce a single system reponse file required to the user. 

## License
This project is provided under the MIT license. See the [LICENSE file](LICENSE) for more info.

## References:
[1] Apostolakis J, Wright DH, Geant4 collaboration. An Overview of the Geant4 Toolkit. AIP Conference Proceedings. 2007. [https://doi.org/10.1063/1.2720452](https://doi.org/10.1063/1.2720452). \
[2] Dubeau J, Hakmana Witharana SS, Atanackovic J, Yonkeu A, Archambault JP. A neutron spectrometer using nested moderators. Radiation Protection Dosimetry. 2012. pp. 217–222. [https://doi.org/10.1093/rpd/ncr381](https://doi.org/10.1093/rpd/ncr381). \
[3] L. Montgomery, M. Evans, L. Liang, R. Maglieri, J. Kildea, "The effect of the flattening filter on photoneutron production at 10 MV in the Varian TrueBeam linear accelerator," *Med Phys*. 2018. [http://dx.doi.org/10.1002/mp.13148](http://dx.doi.org/10.1002/mp.13148). \
[4] R. Maglieri, A. Licea, M. Evans, J. Seuntjens, J. Kildea, "Measuring neutron spectra in radiotherapy using the nested neutrono spectrometer," *Med Phys*. 2015. [https://dx.doi.org/10.1118/1.4931963](https://dx.doi.org/10.1118/1.4931963). 
