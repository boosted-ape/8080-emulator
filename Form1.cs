namespace _8080emu
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            PictureBox spaceInvadersView = new PictureBox();

            Bitmap vramBuffer = new Bitmap(Width, Height);
        }


    }
}