#include <time.h>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include "widget.h"
#include "ui_widget.h"

// Forward declaration of ACESBLUE namespace
namespace ACESBLUE {
extern BluetoothManager *blue;
}

int item1[4][4]=
{
    {0,0,0,0},
    {0,1,1,0},
    {0,1,1,0},
    {0,0,0,0}
};

int item2[4][4]=
{
    {0,1,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,0,0}
};

int item3[4][4]=
{
    {0,0,1,0},
    {0,0,1,0},
    {0,1,1,0},
    {0,0,0,0}
};

int item4[4][4]=
{
    {0,1,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,0,0}
};

int item5[4][4]=
{
    {0,0,1,0},
    {0,1,1,0},
    {0,1,0,0},
    {0,0,0,0}
};

int item6[4][4]=
{
    {0,0,0,0},
    {0,0,1,0},
    {0,1,1,1},
    {0,0,0,0}
};

int item7[4][4]=
{
    {0,0,1,0},
    {0,0,1,0},
    {0,0,1,0},
    {0,0,1,0}
};


inline void block_cpy(int dblock[4][4],int sblock[4][4])
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            dblock[i][j]=sblock[i][j];
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    resize(AREA_COL*BLOCK_SIZE+MARGIN*4+4*BLOCK_SIZE,AREA_ROW*BLOCK_SIZE+MARGIN*2);

    measurement = new MeasurementModule(this, 2);
    ui->exitButton->hide();

    InitGame();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush(QBrush(QColor(112, 128, 144),Qt::SolidPattern));
    painter.drawRect(X_MARGIN,MARGIN,AREA_COL*BLOCK_SIZE,AREA_ROW*BLOCK_SIZE);

    painter.setBrush(QBrush(QColor(112, 128, 144),Qt::SolidPattern));
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(next_block[i][j]==1)
                painter.drawRect(X_MARGIN*1.3+AREA_COL*BLOCK_SIZE+j*BLOCK_SIZE,MARGIN+i*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE);

    painter.setPen(QColor(112, 128, 144));
    painter.setFont(QFont("Arial",20));
    painter.drawText(QRect(X_MARGIN*1.3+AREA_COL*BLOCK_SIZE,MARGIN+4*BLOCK_SIZE,BLOCK_SIZE*5,BLOCK_SIZE*4),Qt::AlignCenter,"score: "+QString::number(score));


    for(int i=0;i<AREA_ROW;i++)
        for(int j=0;j<AREA_COL;j++)
        {
            if(game_area[i][j]==1)
            {
                painter.setBrush(QBrush(QColor(191, 21, 133),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+X_MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
            else if(game_area[i][j]==2)
            {
                painter.setBrush(QBrush(QColor(0, 250, 154),Qt::SolidPattern));
                painter.drawRect(j*BLOCK_SIZE+X_MARGIN,i*BLOCK_SIZE+MARGIN,BLOCK_SIZE,BLOCK_SIZE);
            }
        }
}

void Widget::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==game_timer)
        BlockMove(DOWN);

    if(event->timerId()==paint_timer)
        update();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Up:
        BlockMove(UP);
        break;
    case Qt::Key_R:
        BlockMove(UP);
        break;
    case Qt::Key_W:
        BlockMove(UP);
        break;
    case Qt::Key_Down:
        BlockMove(DOWN);
        break;
    case Qt::Key_S:
        BlockMove(DOWN);
        break;
    case Qt::Key_Left:
        BlockMove(LEFT);
        break;
    case Qt::Key_A:
        BlockMove(LEFT);
        break;
    case Qt::Key_Right:
        BlockMove(RIGHT);
        break;
    case Qt::Key_D:
        BlockMove(RIGHT);
        break;
    case Qt::Key_Space:
        BlockMove(SPACE);
        break;
    default:
        break;
    }
}

void Widget::CreateBlock(int block[4][4],int block_id)
{
    switch (block_id)
    {
    case 0:
        block_cpy(block,item1);
        break;
    case 1:
        block_cpy(block,item2);
        break;
    case 2:
        block_cpy(block,item3);
        break;
    case 3:
        block_cpy(block,item4);
        break;
    case 4:
        block_cpy(block,item5);
        break;
    case 5:
        block_cpy(block,item6);
        break;
    case 6:
        block_cpy(block,item7);
        break;
    default:
        break;
    }
}

void Widget::GetBorder(int block[4][4],Border &border)
{
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            if(block[i][j]==1)
            {
                border.dbound=i;
                break;
            }
    for(int i=3;i>=0;i--)
        for(int j=0;j<4;j++)
            if(block[i][j]==1)
            {
                border.ubound=i;
                break;
            }
    for(int j=0;j<4;j++)
        for(int i=0;i<4;i++)
            if(block[i][j]==1)
            {
                border.rbound=j;
                break;
            }
    for(int j=3;j>=0;j--)
        for(int i=0;i<4;i++)
            if(block[i][j]==1)
            {
                border.lbound=j;
                break;
            }
}

void Widget::InitGame()
{
    for(int i=0;i<AREA_ROW;i++)
        for(int j=0;j<AREA_COL;j++)
            game_area[i][j]=0;

    speed_ms=800;
    refresh_ms=30;

    srand(time(0));

    score=0;
}

void Widget::ResetBlock()
{
    block_cpy(cur_block,next_block);
    GetBorder(cur_block,cur_border);

    int block_id=rand()%7;
    CreateBlock(next_block,block_id);

    block_point start_point;
    start_point.pos_x=AREA_COL/2-2;
    start_point.pos_y=0;
    block_pos=start_point;
    //measurement->logEvent(MeasurementModule::TetNewPiece, ACESBLUE::blue);
}

void Widget::StartGame()
{
    game_timer=startTimer(speed_ms);
    paint_timer=startTimer(refresh_ms);

    measurement->startCount();
    measurement->logEvent(MeasurementModule::TetStart, ACESBLUE::blue);

    int block_id=rand()%7;
    CreateBlock(next_block,block_id);
    ResetBlock();
}

void Widget::GameOver()
{
    EndGame();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Failed");
    msgBox.setText("Game Over");

    // Set the text color using a stylesheet
    msgBox.setStyleSheet("QLabel { color : slategrey; font-size: 22px; }"
                         "QMessageBox { background-color: black; }");

    QAbstractButton *okButton = msgBox.addButton(QMessageBox::Ok);
    okButton->setStyleSheet("background-color: slategrey; border-radius: 15px; color: black; min-width: 120px; min-height: 32px; font-size: 20px;");

    QObject::connect(&msgBox, &QMessageBox::accepted, [&]() {
        emit homeClicked();
    });

    msgBox.exec();
}

void Widget::EndGame()
{
    killTimer(game_timer);
    killTimer(paint_timer);
    measurement->logEvent(MeasurementModule::TetEnd, ACESBLUE::blue);
}

void Widget::BlockRotate(int block[4][4])
{
    int temp_block[4][4];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp_block[3-j][i]=block[i][j];
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            block[i][j]=temp_block[i][j];
}

void Widget::ConvertStable(int x,int y)
{
    for(int i=cur_border.ubound;i<=cur_border.dbound;i++)
        for(int j=cur_border.lbound;j<=cur_border.rbound;j++)
            if(cur_block[i][j]==1)
                game_area[y+i][x+j]=2;

    measurement->logEvent(MeasurementModule::TetPlace, ACESBLUE::blue);
}

bool Widget::IsCollide(int x,int y,Direction dir)
{
    int temp_block[4][4];
    block_cpy(temp_block,cur_block);
    Border temp_border;
    GetBorder(temp_block,temp_border);

    switch(dir)
    {
    case UP:
        BlockRotate(temp_block);
        GetBorder(temp_block,temp_border);
        break;
    case DOWN:
        y+=1;
        break;
    case LEFT:
        x-=1;
        break;
    case RIGHT:
        x+=1;
        break;
    default:
        break;
    }
    for(int i=temp_border.ubound;i<=temp_border.dbound;i++)
        for(int j=temp_border.lbound;j<=temp_border.rbound;j++)
            if(game_area[y+i][x+j]==2&&temp_block[i][j]==1||x+temp_border.lbound<0||x+temp_border.rbound>AREA_COL-1)
                return true;
    return false;
}


void Widget::BlockMove(Direction dir)
{
    switch (dir) {
    case UP:
        if(IsCollide(block_pos.pos_x, block_pos.pos_y, UP))
            break;

        // Save the current block state
        int temp_block[4][4];
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                temp_block[i][j] = game_area[block_pos.pos_y+i][block_pos.pos_x+j];

        // Rotate the block
        BlockRotate(cur_block);

        // Clear previous block position
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                if(temp_block[i][j] == 1 && game_area[block_pos.pos_y+i][block_pos.pos_x+j] != 2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 0;

        // Update game_area with the rotated block
        for(int i=0; i<4; i++)
            for(int j=0; j<4; j++)
                if(cur_block[i][j] == 1)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 1;

        GetBorder(cur_block, cur_border);
        break;

    case DOWN:
        if(block_pos.pos_y+cur_border.dbound==AREA_ROW-1)
        {
            ConvertStable(block_pos.pos_x,block_pos.pos_y);
            ResetBlock();
            break;
        }

        if(IsCollide(block_pos.pos_x,block_pos.pos_y,DOWN))
        {
            ConvertStable(block_pos.pos_x,block_pos.pos_y);
            ResetBlock();
            break;
        }

        // Update game_area with the moved block
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(cur_block[i][j] == 1)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 1;

        block_pos.pos_y+=1;

        // Clear cells above the previous position of the piece
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(block_pos.pos_y-1+i >= 0 && cur_block[i][j] == 1 && game_area[block_pos.pos_y+i][block_pos.pos_x+j] != 2)
                    game_area[block_pos.pos_y-1+i][block_pos.pos_x+j] = 0;



        //block_pos.pos_y+=1;

        for(int i=0;i<4;i++)
            for(int j=cur_border.lbound;j<=cur_border.rbound;j++)
                if(block_pos.pos_y+i<=AREA_ROW-1&&game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=cur_block[i][j];
        break;

    case LEFT:
        if(block_pos.pos_x+cur_border.lbound==0||IsCollide(block_pos.pos_x,block_pos.pos_y,LEFT))
            break;

        // Update game_area with the moved block
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(cur_block[i][j] == 1)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 1;

        block_pos.pos_x-=1;

        // Clear cells to the right of the previous position of the piece
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(cur_block[i][j] == 1 && game_area[block_pos.pos_y+i][block_pos.pos_x+j+1] != 2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j+1] = 0;



        for(int i=cur_border.ubound;i<=cur_border.dbound;i++)
            for(int j=0;j<4;j++)
                if(block_pos.pos_x+j>=0&&game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=cur_block[i][j];
        break;

    case RIGHT:
        if(block_pos.pos_x+cur_border.rbound==AREA_COL-1||IsCollide(block_pos.pos_x,block_pos.pos_y,RIGHT))
            break;

        // Update game_area with the moved block
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(cur_block[i][j] == 1)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 1;

        block_pos.pos_x+=1;

        // Clear cells to the left of the previous position of the piece
        for(int i=0;i<4;i++)
            for(int j=0;j<4;j++)
                if(cur_block[i][j] == 1 && game_area[block_pos.pos_y+i][block_pos.pos_x+j-1] != 2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j-1] = 0;

        for(int i=cur_border.ubound;i<=cur_border.dbound;i++)
            for(int j=0;j<4;j++)
                if(block_pos.pos_x+j<=AREA_COL-1&&game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2)
                    game_area[block_pos.pos_y+i][block_pos.pos_x+j]=cur_block[i][j];
        break;

    case SPACE:
        while(block_pos.pos_y+cur_border.dbound<AREA_ROW-1&&!IsCollide(block_pos.pos_x,block_pos.pos_y,DOWN))
        {
            // Update game_area with the moved block
            for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                    if(cur_block[i][j] == 1)
                        game_area[block_pos.pos_y+i][block_pos.pos_x+j] = 1;

            block_pos.pos_y+=1;

            // Clear cells above the previous position of the piece
            for(int i=0;i<4;i++)
                for(int j=0;j<4;j++)
                    if(block_pos.pos_y-1+i >= 0 && cur_block[i][j] == 1 && game_area[block_pos.pos_y+i][block_pos.pos_x+j] != 2)
                        game_area[block_pos.pos_y-1+i][block_pos.pos_x+j] = 0;

            for(int i=0;i<4;i++)
                for(int j=cur_border.lbound;j<=cur_border.rbound;j++)
                    if(block_pos.pos_y+i<=AREA_ROW-1&&game_area[block_pos.pos_y+i][block_pos.pos_x+j]!=2)
                        game_area[block_pos.pos_y+i][block_pos.pos_x+j]=cur_block[i][j];
        }
        ConvertStable(block_pos.pos_x,block_pos.pos_y);
        ResetBlock();
        break;

    default:
        break;
    }

    int i=AREA_ROW-1;
    int line_count=0;
    while(i>=1)
    {
        bool is_line_full=true;
        for(int j=0;j<AREA_COL;j++)
            if(game_area[i][j]==0)
            {
                is_line_full=false;
                i--;
                break;
            }
        if(is_line_full)
        {
            measurement->logEvent(MeasurementModule::TetRowClear, ACESBLUE::blue);
            for(int k=i;k>=1;k--)
                for(int j=0;j<AREA_COL;j++)
                    game_area[k][j]=game_area[k-1][j];
            line_count++;

            speed_ms -= 50;     // Increase game speed
            if (speed_ms < 100) {
                speed_ms = 100;
            }
            killTimer(game_timer);
            game_timer = startTimer(speed_ms);
            qDebug() << "speed increased. New speed = " << speed_ms;
        }
    }
    score+=line_count*10;

    for(int j=0;j<AREA_COL;j++)
        if(game_area[0][j]==2)
            GameOver();
}


void Widget::on_exitButton_clicked()
{
    EndGame();
    emit homeClicked();
}

