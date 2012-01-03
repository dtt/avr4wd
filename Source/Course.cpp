#include "Course.h"

/**
 * @bried    : �R���X�g���N�^
 * @pram[in] : �Ȃ�
 */
Course::Course(){
	this->setImageSize(320, 240);
	this->inputCoursePoints = new std::vector<CvPoint>();
}
/**
 * @bried    : �f�X�g���N�^
 * @pram[in] : �Ȃ�
 */
Course::~Course(void){
	delete this->inputCoursePoints;
}

/**
 * @bried    : �R�[�X��������ė񋓂���C�e���[�^�̃|�C���^���擾���郁�\�b�h
 * @pram[in] : �Ȃ�
 * @return   : �C�e���[�^�̃|�C���^
 */
CourseIterator* Course::getIterator(){
	return new CourseIterator(this, 0);
}
/**
 * @bried    : �R�[�X��������ė񋓂���C�e���[�^�̃|�C���^���擾���郁�\�b�h (�����n�_�t��)
 * @pram[in] : �C�e���[�^�̏����n�_
 * @return   : �C�e���[�^�̃|�C���^
 */
CourseIterator* Course::getIterator(int indexOfStartPoint){
	return new CourseIterator(this, indexOfStartPoint);
}

/**
 * @bried    : �R�[�X�̉摜�T�C�Y��ݒ肷�郁�\�b�h
 * @pram[in] : �摜�T�C�Y�̉���, ����
 * @return   : �Ȃ�
 */
void Course::setImageSize(int width, int height){
	this->imageSize = cvSize(width, height);
}
/**
 * @bried    : �R�[�X�̉摜�T�C�Y���擾���郁�\�b�h
 * @pram[in] : �Ȃ�
 * @return   : �摜�T�C�Y�̉���, ����
 */
CvSize Course::getImageSize(){
	return this->imageSize;
}

/**
 * @bried    : �R�[�X����Ԃ��֐�
 * @pram[in] : �Ȃ�
 * @return   : �R�[�X�� (�����l)
 */ 
int Course::getCourseLength(){
	return this->inputCoursePoints->size();
}


/**
 * @bried    : �w��C���f�b�N�X�̃R�[�X�_���擾���郁�\�b�h
 * @pram[in] : �C���f�b�N�X�ԍ�
 * @return   : �R�[�X�_�̍��W
 */
CvPoint *Course::getCoursePoint(int index){
	if( index < this->getCourseLength() ){
		return &(this->inputCoursePoints->at(index));
	}
	else{
		return NULL;
	}
}

/**
 * @bried    : �R�[�X�_��ǉ����郁�\�b�h
 * @pram[in] : �ǉ�����R�[�X�_�̍��W
 * @return   : �Ȃ�
 */
void Course::addCoursePoint(CvPoint point){
	this->inputCoursePoints->push_back(point);
}


/**
 * @bried    : �R���X�g���N�^
 * @pram[in] : �R�[�X�̃C���X�^���X�Ə����n�_�̃C���f�b�N�X
 */
CourseIterator::CourseIterator(Course *course, int index){
	this->indexOfStartPoint = index;
	this->courseInstance = course;
	this->index = 0;
}
/**
 * @bried    : �f�X�g���N�^
 * @pram[in] : �Ȃ�
 */
CourseIterator::~CourseIterator(){
}

/**
 * @bried    : ���̗v�f�����݂����true, �����łȂ����false�̐^�U�l��Ԃ����\�b�h
 * @pram[in] : �Ȃ�
 * @return   : �R�[�X��̎��̗v�f�̑��݂ɑ΂���^�U�l
 */
bool CourseIterator::hasNext(){
	if( this->index < this->courseInstance->getCourseLength() ){
		return true;
	}
	else{
		return false;
	}
}

/**
 * @bried    : ���̗v�f�����݂���ꍇ�Ɍ���, �R�[�X��̎��̗v�f���擾���郁�\�b�h (�v hasNext()���\�b�h )
 * @pram[in] : �Ȃ�
 * @return   : �R�[�X��̎��̗v�f�̍��W�̃|�C���^
 */
CvPoint *CourseIterator::getNextPoint(){
	CvPoint *point = this->courseInstance->getCoursePoint( (indexOfStartPoint+index) % this->courseInstance->getCourseLength() );
	index++;
	return point;
}
