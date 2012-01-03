#pragma once

#include <iostream>
#include <stdio.h>
#include <vector>

#include "OpenCV.h"
#include "Calibrator.h"
#include "Course.h"

/**
 * �}�V��������R�[�X�𐶐����郁�\�b�h
 * �R�[�X�f�[�^��XML�`���Ńt�@�C���o�͂�����, �t�@�C��������͂ł��郁�\�b�h
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class CourseEditor
{
public:
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X
	 */ 
	CourseEditor(Calibrator *calibrator, Course *course);
	/**
	 * @bried    : �R���X�g���N�^ (XML�t�@�C�����L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^���擾����)
	 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^���擾����t�@�C����
	 */ 
	CourseEditor(Calibrator *calibrator, Course *course, std::string filename);
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~CourseEditor(void);

	/**
	 * @bried    : XML�t�@�C������L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^��ǂݍ���ŃC���X�^���X�𐶐���, �����̃|�C���^�ɑ�����郁�\�b�h
	 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^���擾����t�@�C����
	 * @return   : �Ȃ�
	 */
	void readFile(Calibrator *calibrator, Course *course, std::string filename);
	/**
	 * @bried    : �L�����u���[�V�����f�[�^�ƃR�[�X�f�[�^��XML�ɏ����o��
	 * @pram[in] : �L�����u���[�V�����N���X�̃C���X�^���X�ƃR�[�X�N���X�̃C���X�^���X, �f�[�^��|���o���t�@�C����
	 * @return   : �Ȃ�
	 */	
	void writeFile(Calibrator *calibrator, Course *course, std::string filename);


	/**
	 * @bried    : �摜�ɃR�[�X�_��`�悷��
	 * @pram[in] : �R�[�X��`�悷�錳�摜�̃|�C���^�ƃR�[�X�N���X�̃|�C���^
	 * @return   : �Ȃ�
	 */ 
	void drawCoursePoints(IplImage *image, Course *course);
	/**
	 * @bried    : �摜�ɃR�[�X�̐���`�悷��
	 * @pram[in] : �R�[�X��`�悷�錳�摜�̃|�C���^�ƃR�[�X�N���X�̃|�C���^
	 * @return   : �Ȃ�
	 */ 
	void drawCourseLines(IplImage *image, Course *course);
	/**
	 * @bried    : �摜�ɃL�����u���[�V�����_��`�悷��
	 * @pram[in] : �`�悷�錳�摜�̃|�C���^�ƃL�����u���[�V�����N���X�̃|�C���^
	 * @return   : �Ȃ�
	 */ 
	void drawCalibrationPoints(IplImage *image, Calibrator *calibrator);

	/**
	 * @bried    : �L�����u���[�V�����Ɋւ���}�E�X�N���b�N�C�x���g�n���h��
	 * @pram[in] : �}�E�X�N���b�N�C�x���g�̓��͒l
	 * @return   : �Ȃ�
	 */
	static void cameraCalibrationEventHandler( int event, int x, int y, int flags ,void *calibratorInstance);
	/**
	 * @bried    : �R�[�X�쐬�ւ���}�E�X�N���b�N�C�x���g�n���h��
	 * @pram[in] : �}�E�X�N���b�N�C�x���g�̓��͒l
	 * @return   : �Ȃ�
	 */
	static void createCourseEventHandler( int event, int x, int y, int flags ,void *courseInstance);

};


