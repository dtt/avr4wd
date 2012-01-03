#pragma once

#include <vector>
#include "OpenCV.h"

class CourseIterator;
class Course;

/**
 * �}�V��������R�[�X���`����N���X
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class Course
{
public: 
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	Course(void);
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~Course(void);

	/**
	 * @bried    : �R�[�X��������ė񋓂���C�e���[�^�̃|�C���^���擾���郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �C�e���[�^�̃|�C���^
	 */
	CourseIterator* getIterator();
	/**
	 * @bried    : �R�[�X��������ė񋓂���C�e���[�^�̃|�C���^���擾���郁�\�b�h (�����n�_�t��)
	 * @pram[in] : �C�e���[�^�̏����n�_
	 * @return   : �C�e���[�^�̃|�C���^
	 */
	CourseIterator* getIterator(int indexOfStartPoint);

	/**
	 * @bried    : �R�[�X�̉摜�T�C�Y��ݒ肷�郁�\�b�h
	 * @pram[in] : �摜�T�C�Y�̉���, ����
	 * @return   : �Ȃ�
	 */
	void setImageSize(int width, int height);
	/**
	 * @bried    : �R�[�X�̉摜�T�C�Y���擾���郁�\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �摜�T�C�Y�̉���, ����
	 */
	CvSize getImageSize();

	/**
	 * @bried    : �R�[�X����Ԃ��֐�
	 * @pram[in] : �Ȃ�
	 * @return   : �R�[�X�� (�����l)
	 */
	int getCourseLength();

	/**
	 * @bried    : �w��C���f�b�N�X�̃R�[�X�_���擾���郁�\�b�h
	 * @pram[in] : �C���f�b�N�X�ԍ�
	 * @return   : �R�[�X�_�̍��W
	 */
	CvPoint *getCoursePoint(int index);
	
	/**
	 * @bried    : �R�[�X�_��ǉ����郁�\�b�h
	 * @pram[in] : �ǉ�����R�[�X�_�̍��W
	 * @return   : �Ȃ�
	 */
	void addCoursePoint(CvPoint point);

	/**
	 * @bried    : �`�F�b�N�|�C���g�ʉ߂Ɣ��肷��~�̔��a
	 */
	static const int radiousThreshold = 200;

protected:
	/**
	 * @bried    : �R�[�X�̉摜�T�C�Y
	 */ 
	CvSize imageSize;

	/**
	 * @bried    : �R�[�X�_�̔z�� (x0, y0), (x1, y1), ..., (xN, yN)
	 */
	std::vector<CvPoint>  *inputCoursePoints;

};


/**
 * �R�[�X������������񂷂�C�e���[�^�N���X
 * ���񂷂鏉���n�_���w�肵�Đ����\
 * @author   : Junpei TSUJI (DT&T)
 * @version  : 1.0.0-2011.12.25
 */
class CourseIterator 
{
public:
	/**
	 * @bried    : �R���X�g���N�^
	 * @pram[in] : �R�[�X�̃C���X�^���X�Ə����n�_�̃C���f�b�N�X
	 */
	CourseIterator(Course *course, int index);
	/**
	 * @bried    : �f�X�g���N�^
	 * @pram[in] : �Ȃ�
	 */
	~CourseIterator();

	/**
	 * @bried    : ���̗v�f�����݂����true, �����łȂ����false�̐^�U�l��Ԃ����\�b�h
	 * @pram[in] : �Ȃ�
	 * @return   : �R�[�X��̎��̗v�f�̑��݂ɑ΂���^�U�l
	 */
	bool hasNext();

	/**
	 * @bried    : ���̗v�f�����݂���ꍇ�Ɍ���, �R�[�X��̎��̗v�f���擾���郁�\�b�h (�v hasNext()���\�b�h )
	 * @pram[in] : �Ȃ�
	 * @return   : �R�[�X��̎��̗v�f�̍��W�̃|�C���^
	 */
	CvPoint *getNextPoint();

private:
	/**
	 * @bried    : ���݂��X�^�[�g���琔���ĉ��Ԗڂ̃|�C���g���\���C���f�b�N�X�ϐ�
	 */
	int index;
	/**
	 * @bried    : �X�^�[�g�n�_�̃C���f�b�N�X
	 */
	int indexOfStartPoint; 
	/**
	 * @bried    : �R�[�X�C���X�^���X�ւ̃|�C���^
	 */ 
	Course *courseInstance;

};
