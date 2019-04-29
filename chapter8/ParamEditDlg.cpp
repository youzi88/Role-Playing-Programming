// ParamEditDlg.cpp : ʵ�ֳ���(implementation file)
//

#include "stdafx.h"
#include "ParamEdit.h"
#include "ParamEditDlg.h"
#include "AddDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParamEditDlg �Ի���(dialog)
CParamEditDlg::CParamEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamEditDlg)
	m_attack = 0;
	m_attack_dist = 0;
	m_defence = 0;
	m_hp = 0;
	m_level = 0;
	m_magic_power = 0;
	m_move_dist = 0;
	m_mp = 0;
	m_resistance = 0;
	m_magic = -1;
	//}}AFX_DATA_INIT
	// ��ע��LoadIcon��Ҫ��Win32��DestroyIcon��������
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CParamEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamEditDlg)
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_ATTACK, m_attack);
	DDV_MinMaxInt(pDX, m_attack, 1, 9999);
	DDX_Text(pDX, IDC_ATTACKDIST, m_attack_dist);
	DDV_MinMaxInt(pDX, m_attack_dist, 1, 10);
	DDX_Text(pDX, IDC_DEFENCE, m_defence);
	DDV_MinMaxInt(pDX, m_defence, 1, 9999);
	DDX_Text(pDX, IDC_HP, m_hp);
	DDV_MinMaxInt(pDX, m_hp, 1, 9999);
	DDX_Text(pDX, IDC_LEVEL, m_level);
	DDV_MinMaxInt(pDX, m_level, 1, 99);
	DDX_Text(pDX, IDC_MAGICPOWER, m_magic_power);
	DDV_MinMaxInt(pDX, m_magic_power, 1, 9999);
	DDX_Text(pDX, IDC_MOVEDIST, m_move_dist);
	DDV_MinMaxInt(pDX, m_move_dist, 1, 10);
	DDX_Text(pDX, IDC_MP, m_mp);
	DDV_MinMaxInt(pDX, m_mp, 1, 9999);
	DDX_Text(pDX, IDC_RESISTANCE, m_resistance);
	DDV_MinMaxInt(pDX, m_resistance, 1, 9999);
	DDX_CBIndex(pDX, IDC_MAGIC, m_magic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CParamEditDlg, CDialog)
	//{{AFX_MSG_MAP(CParamEditDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_NAME, OnSelchangeName)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SET, OnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define PARAMFILE	"parameter.dat"

/////////////////////////////////////////////////////////////////////////////
// CParamEditDlg message handlers �¼��������

BOOL CParamEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �趨�˶Ի����õ�ͼ�ΰ�ť����Ӧ������������ڲ���
	// �Ի���ʱ�����Զ����趨���
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: ���������ر�ĳ�ʼ��ʱ��������������
	// TODO: Add extra initialization here
	params.Load(PARAMFILE);

	char name[16];
	params.FindFirst();
	while (params.FindNext(name)) {
		m_name.AddString(name);
	}
	m_name.SetCurSel(0);
	ChangeName();

	return TRUE;  // ������TRUE�����������������趨�ķ��׹����focus��
	              // return TRUE  unless you set the focus to a control
}

// �����ڶԻ�����������С����ť��������һ�λ���ͼ�ΰ�ť
// �ĳ�����루������ʾ��������MFCӦ�������ʹ��document/view
// ģ�ͣ���˿�ܻ��Զ�ȥ���������������
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CParamEditDlg::OnPaint() 
{
	if (IsIconic())
	{
						   // �����õ�װ�û�����̬��DC��device context��
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// �û��˷��������������
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼʾ
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Ϊ����user��ҷ������С�Ĵ���ʱ����ʾ��꣬
// ϵͳ���������
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CParamEditDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CParamEditDlg::OnOK() 
{
	// TODO:������֤�õĳ������Ҫ�����ڴ�
	// TODO: Add your control notification handler code here
	params.Save(PARAMFILE);

	CDialog::OnOK();
}

void CParamEditDlg::OnSelchangeName() 
{
	// TODO:������֪ͨ��������õĳ������Ҫ�����ڴ�
	// TODO: Add your control notification handler code here	
	ChangeName();
}

void CParamEditDlg::OnAdd() 
{
	// TODO:������֪ͨ��������õĳ������Ҫ�����ڴ�
	// TODO: Add your control notification handler code here
	CAddDlg dlg;
	if (dlg.DoModal() == IDOK) {
		if (m_name.FindStringExact(-1, dlg.m_name) != CB_ERR) {
			MessageBox("����������Ѿ���¼�������ظ���");
		}
		else {
			m_name.SetCurSel(m_name.AddString(dlg.m_name));
			ChangeName();
		}
	}
}

void CParamEditDlg::ChangeName()
{
	int index = m_name.GetCurSel();
	CString str;
	m_name.GetLBText(index, str);
	const CParamFile::parameter *p = params.Find(str);
	if (p == 0) {
		m_attack = 1;
		m_attack_dist = 1;
		m_defence = 1;
		m_hp = 1;
		m_level = 1;
		m_magic_power = 1;
		m_move_dist = 1;
		m_mp = 1;
		m_resistance = 1;
		m_magic = 0;
	}
	else {
		m_attack = p->attack_power;
		m_attack_dist = p->attack_dist;
		m_defence = p->defence_power;
		m_hp = p->hit_point;
		m_level = p->level;
		m_magic_power = p->magic_power;
		m_move_dist = p->move_dist;
		m_mp = p->magic_point;
		m_resistance = p->resistance;
		m_magic = p->magic;
	}
	UpdateData(FALSE);
}

void CParamEditDlg::OnSet() 
{
	// TODO:������֪ͨ��������õĳ������Ҫ�����ڴ�
	// TODO: Add your control notification handler code here

	if (UpdateData(TRUE)) {
		CParamFile::parameter p;

		int index = m_name.GetCurSel();
		CString str;
		m_name.GetLBText(index, str);

		memset(&p, sizeof(p), 0);
		strncpy(p.name, str, 16);
		p.attack_power = m_attack;
		p.attack_dist = m_attack_dist;
		p.defence_power = m_defence;
		p.hit_point = m_hp;
		p.level = m_level;
		p.magic_power = m_magic_power;
		p.move_dist = m_move_dist;
		p.magic_point = m_mp;
		p.resistance = m_resistance;
		p.magic = m_magic;

		params.Add(str, p);
	}
}
