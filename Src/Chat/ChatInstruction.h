
enum class chat_instructions:int
{
	request_register=1,//�ͻ�������ע��
	admit_register,//�������˷���ע������
	reject_register,//�������˾ܾ�ע��
	multi_page_,//��������Ҫ��ͻ���ץȡ��Щҳ�������ͼƬ���� �����Ļḽ���ܶ�����
	single_pic//��������Ҫ��ͻ���ץȡͼƬ ����ḽ���ܶ�����

};
unsigned int magic_number = 0xBAADBAAD;//�����������Ϣ��ǰ��
//Ȼ���ٸ�����������
//Ȼ��ſ��ܸ��ų�����