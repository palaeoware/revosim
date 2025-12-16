#include "imagesequence.h"
#include "mainwindow.h"

//RJG - REvoSim uses image sequences for both its environment and to control variables via linkages.
//Hence this class which includes the functions required to acheive these
ImageSequence::ImageSequence(QStringList files, int rate)
{
    fileList = files;
    changeRate = rate;
    currentFile = 0;
    changeCounter = rate;
    changeForward = true;
    loadFromFile(simulationManager->simulationSettings->environmentMode);

    burnInImage = QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            QColor randomColour(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
            burnInImage.setPixelColor(i, j, randomColour);
        }
}

/*!
 * \brief ImageSequence::loadEnvironmentFromFile
 * \param eMode
 */
void ImageSequence::loadFromFile(int eMode)
//RJG - Load current environment or linkage imagefrom file
{
    //Use make qimage from file method
    //Load the image
    QImage loadImage;
    if (currentFile >= fileList.count()) return;
    else if (currentFile < 0 ) loadImage = burnInImage;
    else loadImage = QImage(fileList[currentFile]);

    if (loadImage.isNull())
    {
        if (!mainWindow->autoFromCommand) QMessageBox::critical(nullptr, "Error", "Fatal - can't open image " + fileList[currentFile]);
        else qInfo() << "Fatal - can't open image " << fileList[currentFile];
        exit(1);
    }
    //Check the size works
    int xsize = loadImage.width();
    int ysize = loadImage.height();

    if (xsize < simulationManager->simulationSettings->gridX || ysize < simulationManager->simulationSettings->gridY) //rescale if necessary - only if too small
        loadImage = loadImage.scaled(QSize(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY), Qt::IgnoreAspectRatio);

    //turn into environment array
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            QRgb colour = loadImage.pixel(i, j);
            grid[i][j][0] = qRed(colour);
            grid[i][j][1] = qGreen(colour);
            grid[i][j][2] = qBlue(colour);
        }

    //set up gridLast - same as environment
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            QRgb colour = loadImage.pixel(i, j);
            gridLast[i][j][0] = qRed(colour);
            gridLast[i][j][1] = qGreen(colour);
            gridLast[i][j][2] = qBlue(colour);
        }

    //set up environment next - depends on eMode
    if (eMode == 0 || fileList.count() == 1)   //static environment
    {
        for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
            for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            {
                QRgb colour = loadImage.pixel(i, j);
                gridNext[i][j][0] = qRed(colour);
                gridNext[i][j][1] = qGreen(colour);
                gridNext[i][j][2] = qBlue(colour);
            }
    }
    else
    {
        //work out next file
        int nextfile = 0;
        if (changeForward)
        {
            if ((currentFile + 1) < fileList.count()) //not yet at end
                nextfile = currentFile + 1;
            else
            {
                //depends on eMode
                if (eMode == 1) nextfile = currentFile; //won't matter
                if (eMode == 2) nextfile = 0; //loop mode
                if (eMode == 3) nextfile = currentFile - 1; //bounce mode
            }
        }
        else     //backwards - simpler, must be eMode 3
        {
            if (currentFile > 0) //not yet at end
                nextfile = currentFile - 1;
            else
                nextfile = 1; //bounce mode
        }

        QImage loadImage2(fileList[nextfile]);
        if (xsize < simulationManager->simulationSettings->gridX || ysize < simulationManager->simulationSettings->gridY) //rescale if necessary - only if too small
            loadImage2 = loadImage2.scaled(QSize(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY), Qt::IgnoreAspectRatio);
        //Get it
        for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
            for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            {
                QRgb colour = loadImage2.pixel(i, j);
                gridNext[i][j][0] = qRed(colour);
                gridNext[i][j][1] = qGreen(colour);
                gridNext[i][j][2] = qBlue(colour);
            }
    }
}

/*!
 * \brief ImageSequence::regenerateEnvironment
 * \param eMode
 * \param interpolate
 * \return
 */
bool ImageSequence::regenerate(int eMode, bool interpolate)
//returns true if finished sim
{
    //RJG - constant environment - either static in menu, or 0 environment change rate, or only one file
    if (changeRate == 0 || eMode == 0 || fileList.count() == 1) return false;

    --changeCounter;

    //is it time to do a full change?
    if (changeCounter <= 0)
    {
        //should not be going backwards!
        if (eMode != 3 && !changeForward) changeForward = true;

        if (changeForward)
        {
            currentFile++; //next image
            if (currentFile >= fileList.count())
            {
                if (eMode == 1) return true; //no more files and we are in 'once' mode - stop the sim
                if (eMode == 2) currentFile = 0; //loop mode
                if (eMode == 3)
                {
                    currentFile -= 2; //bounce mode - back two to undo the extra ++
                    changeForward = false;
                }
            }
        }
        else     //going backwards - must be in eMode 3 (bounce)
        {
            currentFile--; //next image
            if (currentFile < 0)
            {
                currentFile = 1; //bounce mode - one to one again, must have just been 0
                changeForward = true;
            }
        }
        changeCounter = changeRate; //reset counter
        loadFromFile(eMode); //and load it from the file
    }
    else
    {
        if (interpolate)
        {
            float progress, invprogress;
            invprogress = ((float)(changeCounter + 1) / ((float)changeRate));
            progress = 1 - invprogress;
            //not getting new, doing an interpolate
            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    grid[i][j][0] = qint8(0.5 + ((float)gridLast[i][j][0]) * invprogress + ((float)gridNext[i][j][0]) * progress);
                    grid[i][j][1] = qint8(0.5 + ((float)gridLast[i][j][1]) * invprogress + ((float)gridNext[i][j][1]) * progress);
                    grid[i][j][2] = qint8(0.5 + ((float)gridLast[i][j][2]) * invprogress + ((float)gridNext[i][j][2]) * progress);
                }
        }

    }
    return false;
}


/*!
 * \brief ImageSequence::modifyChangeRate
 * \param erate
 * \return
 */
void ImageSequence::modifyChangeRate(int rate)
{
    changeRate = rate;
}


/*!
 * \brief ImageSequence::returnRGB
 * \param n
 * \param m
 * \return
 */
quint8 *ImageSequence::returnRGB(int n, int m)
{
    return grid[n][m];
}


/*!
 * \brief ImageSequence::setRGB
 * \param n
 * \param m
 * \return
 *
 * This is currently used un koad function from mainwindow - ultimately we can do better than this, though, and have a save/load image sequence state function, I suspect
 */
void ImageSequence::setRGB(int n, int m, quint8 *rgb)
{
    grid[n][m][0] = rgb[0];
    grid[n][m][1] = rgb[1];
    grid[n][m][2] = rgb[2];
}

/*!
 * \brief ImageSequence::returnCurrentFileNumber
 * \return
 */
int ImageSequence::returnCurrentFileNumber()
{
    return currentFile;
}

/*!
 * \brief ImageSequence::returnchangeRate
 * \return
 */
int ImageSequence::returnChangeRate()
{
    return changeRate;
}

/*!
 * \brief ImageSequence::returnFileListCount
 * \return
 */
int ImageSequence::returnFileListCount()
{
    return fileList.count();
}


/*!
 * \brief ImageSequence::returnFileList
 * \return
 */
QStringList ImageSequence::returnFileList()
{
    return fileList;
}

/*!
 * \brief ImageSequence::setCurrentFileNumber
 * \param n
 * \return
 */
void ImageSequence::setCurrentFileNumber(int n)
{
    currentFile = n;
}

/*!
 * \brief ImageSequence::returnRGB
 * \param n
 * \param m
 * \return
 */
void ImageSequence::reset(int counter)
{
    changeCounter = counter;
    changeForward = true;
}



