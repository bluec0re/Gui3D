/*
    Gui3DEx
    -------
    
    Copyright (c) 2012 Valentin Frechaud
                                                                                  
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
                                                                                  
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
                                                                                  
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/


#include "Gui3D.h"

namespace Gui3DEx
{

using namespace std;

Gui3D::Gui3D(PanelColors* panelColors)
    : mPanelColors(NULLPTR)
{
    mSilverback = new Gorilla::Silverback();
    setPanelColors(panelColors);
}

Gui3D::Gui3D()
{
    mSilverback = new Gorilla::Silverback();
    mPanelColors = new PanelColors();
}

Gui3D::~Gui3D()
{
    if(mPanelColors)
    {
        delete mPanelColors;
        mPanelColors = NULL;
    }
    // Implicit delete of screens, screenRenderables and atlas
    delete mSilverback;
}

Gorilla::Silverback *Gui3D::getSilverback()
{
    return mSilverback;
}

void Gui3D::setDefaultFont(const std::string &fontName,
                    const std::string &fontFilename,
                    const int fontSize,
                    const int textureSize,
                    const int fontResolution,
                    const bool antiAliased,
                    const Ogre::String &resourceGroup)
{
    if(getSilverback())
    {
        getSilverback()->getFontManager().setDefaultFont(fontName, fontFilename, fontSize, textureSize, fontResolution, antiAliased, resourceGroup);
    }
}

//
// ScreenRenderables methods (2D Screens rendered in 3D)
//
Gorilla::Layer *Gui3D::getLayer(const Ogre::String& screenRenderableName,
                                const Ogre::String& layerName)
{
    if (mScreenRenderables[screenRenderableName] != NULLPTR)
        return mScreenRenderables[screenRenderableName]->mLayers[layerName];
    
    return NULLPTR;
}

Gorilla::Layer *Gui3D::getLayer(Gui3DScreenRenderable *screenRenderable,
                                const Ogre::String& layerName)
{
    map<Ogre::String, Gui3DScreenRenderable *>::iterator it =
        mScreenRenderables.begin();

    for (; it != mScreenRenderables.end(); ++it)
        if (it->second == screenRenderable)
            return getLayer(it->first, layerName);

    return NULLPTR;
}

Gorilla::Layer *Gui3D::createLayer(Gui3DScreenRenderable *screenRenderable,
                                   const Ogre::String &layerName)
{
    // Tests if already exists
    Gorilla::Layer* layer = getLayer(screenRenderable, layerName);
    if (layer != NULLPTR)
        return layer;

    map<Ogre::String, Gui3DScreenRenderable*>::iterator it = 
        mScreenRenderables.begin();

    // Try to find the screenRenderable
    for (; it != mScreenRenderables.end(); ++it)
        if (it->second == screenRenderable)
        {
            Gorilla::Layer* layer = screenRenderable->createLayer(1);
            it->second->mLayers[layerName] = layer;
            return layer;
        }

    return NULLPTR;
}

Gorilla::Layer *Gui3D::createLayer(const Ogre::String &screenRenderableName,
                                   const Ogre::String &layerName)
{
    // Tests if already exists
    Gorilla::Layer* layer = getLayer(screenRenderableName, layerName);
    if (layer != NULLPTR)
        return layer;

    // Try to find the screenRenderable
    Gorilla::ScreenRenderable* screenR = 
        mScreenRenderables[screenRenderableName]->mScreenRenderable;

    if (screenR != NULLPTR)
    {
        Gorilla::Layer* layer = screenR->createLayer();

        mScreenRenderables[screenRenderableName]->mLayers[layerName] = layer;
        return layer;
    }

    return NULLPTR;
}

Gui3DScreenRenderable *Gui3D::createScreenRenderable(const Ogre::Vector2& pos, 
                                                         const Ogre::String& atlasName, 
                                                         const Ogre::String& name)
{
    if (!hasAtlas(atlasName))
        loadAtlas(atlasName);

    mScreenRenderables[name] = new Gui3DScreenRenderable(
        mSilverback->createScreenRenderable(pos, atlasName));

    return mScreenRenderables[name];
}
    
Gui3DScreenRenderable *Gui3D::getScreenRenderable(const Ogre::String& name)
{
    if (mScreenRenderables[name] != NULLPTR)
        return mScreenRenderables[name];

    return NULLPTR;
}


//
// Screens methods (2D screen)
//
Gorilla::Layer *Gui3D::getLayerScreen(const Ogre::String& screenName, 
                                      const Ogre::String& layerName)
{
    if (mScreens[screenName] != 0)
        return mScreens[screenName]->mLayers[layerName];

    return NULLPTR;
}

Gorilla::Layer *Gui3D::getLayerScreen(Gui3DScreen *screen, 
                                      const Ogre::String& layerName)
{
    map<Ogre::String, Gui3DScreen*>::iterator it = mScreens.begin();

    for (; it != mScreens.end(); ++it)
    {
        if (it->second == screen)
            return getLayerScreen(it->first, layerName);
    }

    return NULLPTR;
}

Gorilla::Layer *Gui3D::createLayerScreen(Gui3DScreen *screen, 
                                         const Ogre::String& layerName)
{
    if(!screen)
        return NULLPTR;

    // Tests if already exists
    Gorilla::Layer* layer = getLayerScreen(screen, layerName);
    if (layer != NULLPTR)
        return layer;

    map<Ogre::String, Gui3DScreen *>::iterator it = mScreens.begin();

    // Try to find the screen
    for (; it != mScreens.end(); ++it)
    {
        if (it->second == screen)
        {
            Gorilla::Layer* layer = screen->mScreen->createLayer(1);
            it->second->mLayers[layerName] = layer;
            return layer;
        }
    }

    return NULLPTR;
}

Gorilla::Layer *Gui3D::createLayerScreen(const Ogre::String& screenName, 
                                         const Ogre::String& layerName)
{
    // Tests if already exists
    Gorilla::Layer* layer = getLayerScreen(screenName, layerName);
    if (layer != NULLPTR)
        return layer;

    // Try to find the screen
    Gorilla::Screen* screen = mScreens[screenName]->mScreen;

    if (screen != NULLPTR)
    {
        Gorilla::Layer* layer = screen->createLayer();

        mScreens[screenName]->mLayers[layerName] = layer;
        return layer;
    }

    return NULLPTR;
}

Gui3DScreen *Gui3D::createScreen(Ogre::Viewport* vp, 
                                     const Ogre::String& atlasName, 
                                     const Ogre::String& name)
{
    if (!hasAtlas(atlasName))
        loadAtlas(atlasName);

    mScreens[name] = new Gui3DScreen(mSilverback->createScreen(vp, atlasName));
    return mScreens[name];
}

Gui3DScreen *Gui3D::getScreen(const Ogre::String& name)
{
    if (mScreens[name] != NULLPTR)
        return mScreens[name];
    return NULLPTR;
}


//
// General methods
//
bool Gui3D::hasAtlas(const Ogre::String& name)
{
    for (size_t i=0; i < mAtlas.size(); i++)
        if (mAtlas[i] == name)
            return true;
    return false;
}

void Gui3D::loadAtlas(const Ogre::String& name)
{
    mSilverback->loadAtlas(name);
    mAtlas.push_back(name);
}

void Gui3D::destroyScreenRenderable(const Ogre::String &screenRenderableName)
{
    // Will implicitly destroy its layers
    Gui3DScreenRenderable *screen = mScreenRenderables[screenRenderableName];
    mSilverback->destroyScreenRenderable(screen->mScreenRenderable);
    mScreenRenderables.erase(screenRenderableName);
    delete screen;
}

void Gui3D::destroyScreenRenderable(Gui3DScreenRenderable *screenRenderable)
{
    map<Ogre::String, Gui3DScreenRenderable *>::iterator it =
        mScreenRenderables.begin();

    for (; it != mScreenRenderables.end(); ++it)
        if (it->second == screenRenderable)
        {
            destroyScreenRenderable(it->first);
            break;
        }
}

void Gui3D::destroyScreen(const Ogre::String &screenName)
{
    // Will implicitly destroy its layers
    Gui3DScreen *screen = mScreens[screenName];
    mSilverback->destroyScreen(screen->mScreen);
    mScreens.erase(screenName);
    delete screen;
}

void Gui3D::destroyScreen(Gui3DScreen *screen)
{
    map<Ogre::String, Gui3DScreen*>::iterator it =
        mScreens.begin();

    for (; it != mScreens.end(); ++it)
        if (it->second == screen)
        {
            destroyScreen(it->first);
            break;
        }
}

void Gui3D::setPanelColors(PanelColors* panelColors)
{
    if(mPanelColors)
    {
        delete mPanelColors;
    }
    mPanelColors = panelColors;
}

PanelColors *Gui3D::getPanelColors()
{
    return mPanelColors;
}

}