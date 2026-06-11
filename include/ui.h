// Declares reusable UI widgets and file-saving helpers.

#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Button
{
private:
    sf::RectangleShape shape;
    const sf::Font* fontPtr;
    string labelText;
    unsigned int labelSize;
    sf::Color labelColor;

public:
    // Creates an uninitialized button placeholder.
    Button();

    // Creates a button with position, size, label, and font.
    Button(float x, float y, float width, float height,
           const string& text, const sf::Font& font, unsigned int textSize);

    // Moves the button.
    void setPosition(float x, float y);
    // Resizes the button.
    void setSize(float width, float height);
    // Sets the button label.
    void setText(const string& text);
    // Sets the button colors.
    void setColors(const sf::Color& fillColor,
                   const sf::Color& outlineColor,
                   const sf::Color& textColor);
    // Sets the button outline thickness.
    void setOutlineThickness(float thickness);

    // Checks whether a point is inside the button.
    bool containsPoint(float mouseX, float mouseY) const;
    // Checks whether an SFML event clicked the button.
    bool isClicked(const sf::Event& event) const;

    // Draws the button.
    void draw(sf::RenderWindow& window) const;
};

class TextBox
{
private:
    sf::RectangleShape shape;
    const sf::Font* fontPtr;
    string content;
    unsigned int textSize;
    unsigned int maxLength;
    sf::Color textColor;
    bool selected;

public:
    // Creates an uninitialized textbox placeholder.
    TextBox();

    // Creates a numeric textbox with size and input limit.
    TextBox(float x, float y, float width, float height,
            const sf::Font& font, unsigned int textSize, unsigned int maxLength);

    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setColors(const sf::Color& fillColor,
                   const sf::Color& outlineColor,
                   const sf::Color& textColor);

    // Sets textbox content.
    void setText(const string& newText);
    // Returns textbox content.
    string getText() const;

    // Sets textbox focus state.
    void setSelected(bool isSelected);
    // Returns textbox focus state.
    bool isSelected() const;

    bool containsPoint(float mouseX, float mouseY) const;

    // Updates focus from a mouse click.
    void handleMouseClick(const sf::Event& event);
    // Applies numeric keyboard input.
    void handleTextInput(const sf::Event& event);

    void draw(sf::RenderWindow& window) const;
};

// Writes text content to a file.
bool saveToFile(const string& fileName, const string& content);
// Returns a timestamp for saved file names.
string getTimestampString();

#endif
