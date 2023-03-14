# Biomass Sensor Project

The goal of this project is to improve the measurement and prediction of algae density inside of outdoor tanks used in HBOI's IMTA system.

![tanks](data/tank_layout.png)

## Hardware

Custom sensors have been designed to automatically record alage density. The current sensor design is split into two main components.

The **sensor head** component is located underwater and is responsible for taking the actual algae density measurements.

The **topside** component is located above water and broadcasts the data collected by teh sensor.

In addition to the deployed sensors, the project also utilizes a central basetation (RPI-3A) to collect and upload all of the data to **Firebase**.

The folders are broken down as follows:

* [biomass_ble](/biomass_ble/): code for the version 1 algae sensor (all-in-one sensor).
* [topside](/topside/): code for the topside component.
* sensor_head: code for the sensor head component.
* [pc_basestation](/pc_basestation/): code for the basestation.

## Data

This repository contains a set of data collected by our version 1 algae sensor found in the [data folder](/data/). The code for processing the data is also found in the same folder with more information available [here](/data/README.md).

## Prediction

There is currently no code available for predicting algae density.
