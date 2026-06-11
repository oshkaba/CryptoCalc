// Reusable UI widgets and save-file helpers for CryptoCalc.
// Button and TextBox keep drawing/input behavior out of the app controller.

#include "ui.h"
#include <ctime>
#include <fstream>

// Creates an empty button used before real layout values are assigned.
Button::Button()
{
    fontPtr = nullptr;
    labelText = "";
    labelSize = 20;
    labelColor = sf::Color::White;

    shape.setPosition(sf::Vector2f(0.0f, 0.0f));
    shape.setSize(sf::Vector2f(0.0f, 0.0f));
    shape.setFillColor(sf::Color(48, 48, 48));
    shape.setOutlineColor(sf::Color(148, 163, 184));
    shape.setOutlineThickness(2.0f);
}

// Creates a positioned button with text and font settings.
Button::Button(float x, float y, float width, float height, const string& text, const sf::Font& font, unsigned int textSize)
{
    fontPtr = &font;
    labelText = text;
    labelSize = textSize;
    labelColor = sf::Color::White;

    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(48, 48, 48));
    shape.setOutlineColor(sf::Color(148, 163, 184));
    shape.setOutlineThickness(2.0f);
}

// Moves the button rectangle to a new screen position.
void Button::setPosition(float x, float y)
{
    shape.setPosition(sf::Vector2f(x, y));
}

// Resizes the button rectangle without changing its label.
void Button::setSize(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
}

// Updates the visible button label.
void Button::setText(const string& text)
{
    labelText = text;
}

// Applies fill, outline, and text colors to the button.
void Button::setColors(const sf::Color& fillColor, const sf::Color& outlineColor, const sf::Color& textColor)
{
    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    labelColor = textColor;
}

// Changes the button border thickness.
void Button::setOutlineThickness(float thickness)
{
    shape.setOutlineThickness(thickness);
}

// Checks whether a mouse position is inside the button bounds.
bool Button::containsPoint(float mouseX, float mouseY) const
{
    return shape.getGlobalBounds().contains(sf::Vector2f(mouseX, mouseY));
}

// Returns true only for a left-click inside the button.
bool Button::isClicked(const sf::Event& event) const
{
    if(const sf::Event::MouseButtonPressed* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if(mousePressed->button == sf::Mouse::Button::Left)
        {
            return containsPoint(static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y));
        }
    }

    return false;
}

// Draws the button and centers its label inside the rectangle.
void Button::draw(sf::RenderWindow& window) const
{
    window.draw(shape);

    if(fontPtr == nullptr)
    {
        return;
    }

    sf::Text label(*fontPtr, labelText, labelSize);
    label.setFillColor(labelColor);

    sf::FloatRect textBounds = label.getLocalBounds();
    sf::FloatRect buttonBounds = shape.getGlobalBounds();

    float textX = buttonBounds.position.x + (buttonBounds.size.x - textBounds.size.x) / 2.0f - textBounds.position.x;
    float textY = buttonBounds.position.y + (buttonBounds.size.y - textBounds.size.y) / 2.0f - textBounds.position.y;

    label.setPosition(sf::Vector2f(textX, textY));

    window.draw(label);
}



// Creates an empty textbox used before real layout values are assigned.
TextBox::TextBox()
{
    fontPtr = nullptr;
    content = "";
    textSize = 20;
    maxLength = 20;
    textColor = sf::Color::White;
    selected = false;

    shape.setPosition(sf::Vector2f(0.0f, 0.0f));
    shape.setSize(sf::Vector2f(0.0f, 0.0f));
    shape.setFillColor(sf::Color(33, 33, 33));
    shape.setOutlineColor(sf::Color(148, 163, 184));
    shape.setOutlineThickness(2.0f);
}

// Creates a positioned numeric textbox with a maximum input length.
TextBox::TextBox(float x, float y, float width, float height, const sf::Font& font, unsigned int inputTextSize, unsigned int inputMaxLength)
{
    fontPtr = &font;
    content = "";
    textSize = inputTextSize;
    maxLength = inputMaxLength;
    textColor = sf::Color::White;
    selected = false;

    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(33, 33, 33));
    shape.setOutlineColor(sf::Color(148, 163, 184));
    shape.setOutlineThickness(2.0f);
}

// Moves the textbox to a new screen position.
void TextBox::setPosition(float x, float y)
{
    shape.setPosition(sf::Vector2f(x, y));
}

// Resizes the textbox rectangle.
void TextBox::setSize(float width, float height)
{
    shape.setSize(sf::Vector2f(width, height));
}

// Applies fill, outline, and text colors to the textbox.
void TextBox::setColors(const sf::Color& fillColor, const sf::Color& outlineColor, const sf::Color& newTextColor)
{
    shape.setFillColor(fillColor);
    shape.setOutlineColor(outlineColor);
    textColor = newTextColor;
}

// Sets textbox content and trims it to the configured maximum length.
void TextBox::setText(const string& newText)
{
    content = newText;

    if(content.size() > maxLength)
    {
        content = content.substr(0, maxLength);
    }
}

// Returns the current textbox content.
string TextBox::getText() const
{
    return content;
}

// Marks the textbox as active and updates its outline color.
void TextBox::setSelected(bool isSelected)
{
    selected = isSelected;

    if(selected)
    {
        shape.setOutlineColor(sf::Color(16, 185, 129));
    }
    else
    {
        shape.setOutlineColor(sf::Color(148, 163, 184));
    }
}

// Reports whether the textbox currently receives typed input.
bool TextBox::isSelected() const
{
    return selected;
}

// Checks whether a mouse position is inside the textbox bounds.
bool TextBox::containsPoint(float mouseX, float mouseY) const
{
    return shape.getGlobalBounds().contains(sf::Vector2f(mouseX, mouseY));
}

// Selects the textbox when it is clicked and deselects it otherwise.
void TextBox::handleMouseClick(const sf::Event& event)
{
    if(const sf::Event::MouseButtonPressed* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if(mousePressed->button == sf::Mouse::Button::Left)
        {
            if(containsPoint(static_cast<float>(mousePressed->position.x), static_cast<float>(mousePressed->position.y)))
            {
                setSelected(true);
            }
            else
            {
                setSelected(false);
            }
        }
    }
}

// Accepts digits and backspace while rejecting non-numeric input.
void TextBox::handleTextInput(const sf::Event& event)
{
    if(!selected)
    {
        return;
    }

    if(const sf::Event::TextEntered* textEntered = event.getIf<sf::Event::TextEntered>())
    {
        unsigned int unicode = textEntered->unicode;

        if(unicode == 8)
        {
            if(!content.empty())
            {
                content.erase(content.size() - 1, 1);
            }
        }
        else if(unicode >= '0' && unicode <= '9')
        {
            if(content.size() < maxLength)
            {
                content += static_cast<char>(unicode);
            }
        }
    }
}

// Draws the textbox and vertically centers its current text.
void TextBox::draw(sf::RenderWindow& window) const
{
    window.draw(shape);

    if(fontPtr == nullptr)
    {
        return;
    }

    sf::Text displayText(*fontPtr, content, textSize);
    displayText.setFillColor(textColor);

    sf::FloatRect textBounds = displayText.getLocalBounds();
    sf::FloatRect boxBounds = shape.getGlobalBounds();

    float textX = boxBounds.position.x + 8.0f;
    float textY = boxBounds.position.y + (boxBounds.size.y - textBounds.size.y) / 2.0f - textBounds.position.y + 1.0f;

    displayText.setPosition(sf::Vector2f(textX, textY));

    window.draw(displayText);
}



// Writes output text to disk and reports whether the write succeeded.
bool saveToFile(const string& fileName, const string& content)
{
    ofstream outFile(fileName);

    if(!outFile.is_open())
    {
        return false;
    }

    outFile << content;
    outFile.close();

    return true;
}

// Produces a filesystem-safe timestamp for saved result filenames.
string getTimestampString()
{
    time_t now = time(0);
    tm* localTime = localtime(&now);

    string timestamp = "";

    timestamp += to_string(localTime->tm_year + 1900);
    timestamp += "-";

    if(localTime->tm_mon + 1 < 10)
    {
        timestamp += "0";
    }
    timestamp += to_string(localTime->tm_mon + 1);
    timestamp += "-";

    if(localTime->tm_mday < 10)
    {
        timestamp += "0";
    }
    timestamp += to_string(localTime->tm_mday);
    timestamp += "_";

    if(localTime->tm_hour < 10)
    {
        timestamp += "0";
    }
    timestamp += to_string(localTime->tm_hour);
    timestamp += "-";

    if(localTime->tm_min < 10)
    {
        timestamp += "0";
    }
    timestamp += to_string(localTime->tm_min);
    timestamp += "-";

    if(localTime->tm_sec < 10)
    {
        timestamp += "0";
    }
    timestamp += to_string(localTime->tm_sec);

    return timestamp;
}
