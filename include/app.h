// Declares the main CryptoCalc application class and screen state.
// The implementation in app.cpp owns rendering, events, and tool actions.

#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include <string>

#include "ui.h"

using namespace std;

enum Screen
{
    HOME_SCREEN,
    PRIME_SCREEN,
    RSA_SCREEN,
    DH_SCREEN,
    KEY_TOOLS_SCREEN,
    BENCHMARK_SCREEN
};

class App
{
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Font boldFont;
    sf::Font monoFont;

    Screen currentScreen;
    string lastResultText;
    bool rsaOutputReady;
    bool dhOutputReady;
    string rsaKeyColumn;
    string rsaEncryptColumn;
    string rsaDecryptColumn;
    string dhSetupColumn;
    string dhAliceColumn;
    string dhBobColumn;

    Button homeButton;
    Button primeButton;
    Button rsaButton;
    Button dhButton;
    Button keyToolsButton;
    Button benchmarkButton;
    Button saveButton;
    Button exitButton;

    Button primeRunButton;
    Button rsaRunButton;
    Button dhRunButton;
    Button gcdToolButton;
    Button inverseToolButton;
    Button modExpToolButton;
    Button rsaBenchmarkButton;
    Button sieveBenchmarkButton;

    TextBox primeInputBox;

    TextBox rsaMinPrimeBox;
    TextBox rsaMaxPrimeBox;
    TextBox rsaMessageBox;

    TextBox dhPBox;
    TextBox dhGBox;
    TextBox dhABox;
    TextBox dhBBox;

    TextBox gcdABox;
    TextBox gcdBBox;

    TextBox inverseABox;
    TextBox inverseModBox;

    TextBox powerBaseBox;
    TextBox powerExponentBox;
    TextBox powerModBox;

    TextBox rsaBenchmarkRunsBox;
    TextBox sieveBenchmarkMaxBox;

public:
    // Sets up the application window and initializes all UI state.
    App();
    // Starts the application loop.
    void run();

private:
    // Loads fonts used by the UI and output tables.
    void loadFont();
    // Builds all sidebar and screen action buttons.
    void createButtons();
    // Builds all numeric input boxes with their defaults.
    void createTextBoxes();

    // Reads window events from SFML.
    void processEvents();
    // Routes mouse clicks to buttons and text boxes.
    void handleMouseClick(const sf::Event& event);
    // Routes typed numeric input to the selected text box.
    void handleTextInput(const sf::Event& event);

    // Reserved for future per-frame state changes.
    void update();
    // Draws the full current frame.
    void render();

    // Draws the persistent navigation sidebar.
    void drawSidebar();
    // Draws the active tool screen and output panel.
    void drawMainArea();
    // Draws the shared calculation output area.
    void drawOutputPanel();

    // Draws the home dashboard.
    void drawHomeScreen();
    // Draws the primality checker screen.
    void drawPrimeScreen();
    // Draws the RSA screen.
    void drawRSAScreen();
    // Draws the Diffie-Hellman screen.
    void drawDHScreen();
    // Draws the GCD, inverse, and power-mod tools.
    void drawKeyToolsScreen();
    // Draws the benchmark controls.
    void drawBenchmarkScreen();

    // Draws text with the default UI font.
    void drawText(const string& text, float x, float y, unsigned int size, const sf::Color& color);
    // Draws text with a caller-selected font.
    void drawTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color);
    // Draws text centered inside a horizontal region.
    void drawCenteredText(const string& text, float boxX, float y, float boxWidth, unsigned int size, const sf::Color& color);
    // Draws wrapped text with the default font.
    void drawWrappedText(const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxChars, int maxLines);
    // Draws wrapped text with a caller-selected font.
    void drawWrappedTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxChars, int maxLines);
    // Draws unwrapped monospaced lines for aligned tables.
    void drawPlainTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxLines);
    // Draws RSA output as three columns.
    void drawRSAOutputColumns();
    // Draws Diffie-Hellman output as three columns.
    void drawDHOutputColumns();
    // Draws RSA formulas with positioned exponents.
    void drawRSAFormula(float x, float y);
    // Clears saved output text and column state.
    void clearOutput();

    // Runs primality checking from screen input.
    void runPrimeTestFromScreen();
    // Runs RSA from screen input.
    void runRSAFromScreen();
    // Runs Diffie-Hellman from screen input.
    void runDHFromScreen();
    // Runs the GCD calculator.
    void runGCDToolFromScreen();
    // Runs the modular inverse calculator.
    void runInverseToolFromScreen();
    // Runs the modular exponent calculator.
    void runModExpToolFromScreen();
    // Runs the RSA benchmark from screen input.
    void runRSABenchmarkFromScreen();
    // Runs the sieve benchmark from screen input.
    void runSieveBenchmarkFromScreen();
    // Saves the current output text.
    void saveLastResultToFile();

    // Parses a non-negative integer from textbox content.
    bool stringToLongLong(const string& text, long long& value);
    // Returns the current screen name.
    string currentScreenName() const;
};

#endif