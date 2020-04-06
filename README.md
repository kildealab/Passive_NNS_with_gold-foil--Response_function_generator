# Passive_NNS_with_gold-foil---Response_functions_generator
This is a Monte-Carlo simulation package based on GEANT4 [1] to generate the system response functions of a passive Nested Neutron Spectrometer (NNS) [2]. Passive NNS consists of seven cylindrical HDPE moderator shells and a gold-foil as a thermal neutron detector. The gold-foil is positioned at the geometrical centre of the moderators in a horizontal disposition using two cylindrical inserts. This spectrometer can be used to measure the neutron flux spectra but the spectrometer response functions are essential to unfold the neutron fulx spectra from the measurements. This software will enable the passive NNS user to generate the system response functions. 

## Author
Felix Mathew
Contact email: felix.mathew@mail.mcgill.ca

## Prerequisites
* Geant4.10.04.p02 ( See [the installation guide](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/InstallationGuide/BackupVersions/V10.4/html/index.html) to install Geant4)

## Program structure

## Features:
* User can choose the moderator configuration to be used in the simulation
* Simulates 10<sup>7</sup> mono-energetic neutron sources in each run
* Multithreaded to enable parallel runs
* Importance sampling variance reduction technique included
* User can switch between two scorers (neutron capture scorer and neutron cellflux scorer)
* Individual simulation involve 52 mono-energetic runs spanning 1 meV to 16 MeV

## Getting started


## License
This project is provided under the MIT license. See the [LICENSE file](LICENSE) for more info.

## References:
[1] Apostolakis J, Wright DH, Geant4 collaboration. An Overview of the Geant4 Toolkit. AIP Conference Proceedings. 2007. [https://doi.org/10.1063/1.2720452](https://doi.org/10.1063/1.2720452).
[2] Dubeau J, Hakmana Witharana SS, Atanackovic J, Yonkeu A, Archambault JP. A neutron spectrometer using nested moderators. Radiation Protection Dosimetry. 2012. pp. 217–222. [https://doi.org/10.1093/rpd/ncr381](https://doi.org/10.1093/rpd/ncr381).
