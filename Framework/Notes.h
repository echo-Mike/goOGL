/*
	���� �������� - ��������� ������:
		* �������� - SOIL
		* ��� - ASSIMP
		* �������� - ASSIMP
		* ������ - Native
		* ����� - OpenAL
		* �������� ����� - ASSIMP
		* ������ - ASSIMP

		* ������������� ������ - GLM + Native
		* ����� - Native

		* ������ - ASSIMP
		* �������� - ASSIMP
		* ����� - ASSIMP
		
		* ������ - Create
		* ������ - Create
		
		* ����������� ����� - GLFW
		* ���� ���������� - GLFW
		* �����-����� - OpenAL

		* ������ - ?
		* ����� - ?

	� ��������� ������� ����������:
		������ - ��������������� ������, ������� �������� �� ������� ��������� ��������.
			� ������ ������ �� ����������� � ��������� ���������.
			��� ���������� ������ ������� ���������� ������������ ��� ��� ����������� ��� ��������� ������� ����������.
			������ ��������:
				* �������� ���������� � ������������ ��������� � ogl ������� (��� �������� ���������� "������") - 
						������������ ��� ������, ����������� ��� ������ ������� ��������� ������������� �������.
				* ������������� ��������� ������ � ��������, ��� ������� ��� ������ ��� ������������ �
					UniformAutomaticInteface � ��� ��������� � ������� ��������. -
						������������ ��� ������, ����������� ��� ������ ������� ��������� ������������� �������.
				* ��������� ��� ������� � ����� � ������������� ��� � ������� ��������� ogl

		��� - ������������ �������(��������) ������:
				* ��������� (vertex);
				* ��������� (faces);
				* ��������;
				* ���������� ���������;
				* ���������;
				* �����������;
			� ����� ���� ����� ���� ����� ���������:
				������� ���� - ��������� ������, ����������� ��� ����������� ��������� ������ ���� (�������� ������ � ���������).
			��� ����� ������ ������ ������������ ����� � ����������.
			��� ��������� �:
				* ����������� �������� ������ ���� � VAO;
				* ���������� ���������, ������������ ����� ����������;
				* ������ ������� ������������� ������� (��� ��������� - �������� ���� ����������������� � ������� ������ ������);
				* �������������� �������� ��������� ������� ��������, ��� ������ ��� ���������;
				* ��������� VAO;
				* ������ ������� ��������� ��������� (glDrawElements/glDrawArrays);
				* � ����� ����: ���������� ����������� ���������, ������������ � ����.

		���������� ����� ������ - ������, ���������� �� ������� � �����-�������


*/