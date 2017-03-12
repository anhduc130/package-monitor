/*
 * camera.h
 *
 *  Created on: Feb 4, 2017
 *      Author: David
 */

#include <stdint.h>

#ifndef CAMERA_H_
#define CAMERA_H_

#define VC0706_RESET  0x26
#define VC0706_GEN_VERSION 0x11
#define VC0706_SET_PORT 0x24
#define VC0706_READ_FBUF 0x32
#define VC0706_GET_FBUF_LEN 0x34
#define VC0706_FBUF_CTRL 0x36
#define VC0706_DOWNSIZE_CTRL 0x54
#define VC0706_DOWNSIZE_STATUS 0x55
#define VC0706_READ_DATA 0x30
#define VC0706_WRITE_DATA 0x31
#define VC0706_COMM_MOTION_CTRL 0x37
#define VC0706_COMM_MOTION_STATUS 0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MOTION_CTRL 0x42
#define VC0706_MOTION_STATUS 0x43
#define VC0706_TVOUT_CTRL 0x44
#define VC0706_OSD_ADD_CHAR 0x45

#define VC0706_STOPCURRENTFRAME 0x0
#define VC0706_STOPNEXTFRAME 0x1
#define VC0706_RESUMEFRAME 0x3
#define VC0706_STEPFRAME 0x2

#define VC0706_640x480 0x00
#define VC0706_320x240 0x11
#define VC0706_160x120 0x22

#define VC0706_MOTIONCONTROL 0x0
#define VC0706_UARTMOTION 0x01
#define VC0706_ACTIVATEMOTION 0x01

#define VC0706_SET_ZOOM 0x52
#define VC0706_GET_ZOOM 0x53

#define CAMERABUFFSIZ 100
#define CAMERADELAY 10

#define true 1
#define false 0

/*****************************************************************************
**  Camera Registers
*****************************************************************************/
#define Camera_Control	(*(volatile uint8_t *)(0x84000250))
#define Camera_Status	(*(volatile uint8_t *)(0x84000250))
#define Camera_TxData	(*(volatile uint8_t *)(0x84000252))
#define Camera_RxData	(*(volatile uint8_t *)(0x84000252))
#define Camera_Baud		(*(volatile uint8_t *)(0x84000254))

uint8_t  cameraImage[20000];
int doneFlag;
int imageLengthInt;
uint8_t imageLength[2];

/*******************************************************************************************
** Initialize the camera
*******************************************************************************************/
void Camera_Init(void);

/*******************************************************************************************
** Reset the camera
*******************************************************************************************/
int reset(void);

/*******************************************************************************************
** Set the size of the image
*******************************************************************************************/
int changeImageSize(void);

/*******************************************************************************************
 ** Get the length of the image
 *******************************************************************************************/
int getImageLength(void);

/*******************************************************************************************
** Make the camera take a picture
*******************************************************************************************/
int takePicture(void);

/*******************************************************************************************
** Read the picture that the camera took, store it in a buffer
*******************************************************************************************/
int readImage(void);

/*******************************************************************************************
** Read a byte from 6850
*******************************************************************************************/
uint8_t readRx(void);

/*******************************************************************************************
** Send a byte to 6850
*******************************************************************************************/
void writeTx(uint8_t);

/*******************************************************************************************
** Stop taking the picture
*******************************************************************************************/
int stopTakingImage(void);

#endif /* CAMERA_H_ */
