// Main SFML application controller for CryptoCalc.
// Performs screen routing, input handling, drawing, and user actions.
#include "app.h"
#include <sstream>
#include <optional>
#include <cctype>
#include <iostream>
#include "math_utils.h"
#include "rsa.h"
#include "dh.h"
#include "benchmark.h"
#include "ui.h"

using namespace std;

static const int MAX_INPUT_DIGITS = 8;
static const long long MAX_RSA_RANGE_WIDTH = 100000;

static const sf::Color COLOR_APP_BG(229, 231, 235);
static const sf::Color COLOR_SIDEBAR(17, 17, 17);
static const sf::Color COLOR_SIDEBAR_BUTTON(30, 30, 30);
static const sf::Color COLOR_SIDEBAR_BORDER(55, 65, 81);
static const sf::Color COLOR_SIDEBAR_TEXT(245, 245, 245);
static const sf::Color COLOR_MAIN(243, 244, 246);
static const sf::Color COLOR_OUTPUT(255, 255, 255);
static const sf::Color COLOR_INPUT(255, 255, 255);
static const sf::Color COLOR_TEXT(17, 24, 39);
static const sf::Color COLOR_MUTED(55, 65, 81);
static const sf::Color COLOR_LINE(148, 163, 184);
static const sf::Color COLOR_CYAN(2, 132, 199);
static const sf::Color COLOR_BLUE(37, 99, 235);
static const sf::Color COLOR_GREEN(5, 150, 105);
static const sf::Color COLOR_PURPLE(124, 58, 237);
static const sf::Color COLOR_AMBER(217, 119, 6);
static const sf::Color COLOR_ROSE(220, 38, 38);
static const sf::Color COLOR_CARD(255, 255, 255);

// Initializes the application window, screen state, fonts, and UI widgets.
App::App() : window(sf::VideoMode({1200, 750}), "CryptoCalc")
{
    currentScreen = HOME_SCREEN;
    lastResultText = "";
    rsaOutputReady = false;
    dhOutputReady = false;
    rsaKeyColumn = "";
    rsaEncryptColumn = "";
    rsaDecryptColumn = "";
    dhSetupColumn = "";
    dhAliceColumn = "";
    dhBobColumn = "";

    loadFont();
    createButtons();
    createTextBoxes();
}

// Runs the main event/render loop until the window closes.
void App::run()
{
    while(window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

// Loads the UI, bold, and monospaced fonts from local project font files.
void App::loadFont()
{
    if(!font.openFromFile("fonts/segoeui.ttf"))
    {
        cerr << "Could not load font: fonts/segoeui.ttf" << endl;
    }

    if(!boldFont.openFromFile("fonts/segoeuib.ttf"))
    {
        cerr << "Could not load font: fonts/segoeuib.ttf" << endl;

        if(!boldFont.openFromFile("fonts/segoeui.ttf"))
        {
            cerr << "Could not load fallback bold font: fonts/segoeui.ttf" << endl;
        }
    }

    if(!monoFont.openFromFile("fonts/consola.ttf"))
    {
        cerr << "Could not load font: fonts/consola.ttf" << endl;

        if(!monoFont.openFromFile("fonts/cour.ttf"))
        {
            cerr << "Could not load fallback mono font: fonts/cour.ttf" << endl;
        }
    }
}

// Creates all navigation and action buttons used by the app screens.
void App::createButtons()
{
    homeButton = Button(20, 20, 180, 45, "Home", font, 21);
    primeButton = Button(20, 80, 180, 45, "Primality Test", font, 21);
    rsaButton = Button(20, 140, 180, 45, "RSA", font, 21);
    dhButton = Button(20, 200, 180, 45, "Diffie-Hellman", font, 21);
    keyToolsButton = Button(20, 260, 180, 45, "Math Tools", font, 21);
    benchmarkButton = Button(20, 320, 180, 45, "Benchmarks", font, 21);
    saveButton = Button(20, 380, 180, 45, "Save Result", font, 21);
    exitButton = Button(20, 440, 180, 45, "Exit", font, 21);

    primeRunButton = Button(260, 205, 180, 45, "Test Prime", font, 20);
    rsaRunButton = Button(260, 335, 180, 45, "Run RSA", font, 20);
    dhRunButton = Button(500, 330, 180, 38, "Run DH", font, 20);
    gcdToolButton = Button(850, 142, 170, 38, "Compute GCD", font, 17);
    inverseToolButton = Button(850, 242, 170, 38, "Find Inverse", font, 17);
    modExpToolButton = Button(850, 342, 170, 38, "Power Mod", font, 17);
    rsaBenchmarkButton = Button(260, 185, 230, 45, "RSA Benchmark", font, 20);
    sieveBenchmarkButton = Button(260, 270, 230, 45, "Sieve Benchmark", font, 20);

    homeButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    primeButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    rsaButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    dhButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    keyToolsButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    benchmarkButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    saveButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);
    exitButton.setColors(COLOR_SIDEBAR_BUTTON, COLOR_SIDEBAR_BORDER, COLOR_SIDEBAR_TEXT);

    primeRunButton.setColors(COLOR_GREEN, COLOR_GREEN, sf::Color::White);
    rsaRunButton.setColors(COLOR_GREEN, COLOR_GREEN, sf::Color::White);
    dhRunButton.setColors(COLOR_GREEN, COLOR_GREEN, sf::Color::White);
    gcdToolButton.setColors(COLOR_PURPLE, COLOR_PURPLE, sf::Color::White);
    inverseToolButton.setColors(COLOR_PURPLE, COLOR_PURPLE, sf::Color::White);
    modExpToolButton.setColors(COLOR_PURPLE, COLOR_PURPLE, sf::Color::White);
    rsaBenchmarkButton.setColors(COLOR_AMBER, COLOR_AMBER, sf::Color::White);
    sieveBenchmarkButton.setColors(COLOR_AMBER, COLOR_AMBER, sf::Color::White);
}

// Creates text boxes with fixed positions, defaults, and input limits.
void App::createTextBoxes()
{
    primeInputBox = TextBox(260, 150, 220, 40, font, 22, MAX_INPUT_DIGITS);

    rsaMinPrimeBox = TextBox(260, 140, 180, 40, font, 22, MAX_INPUT_DIGITS);
    rsaMaxPrimeBox = TextBox(260, 210, 180, 40, font, 22, MAX_INPUT_DIGITS);
    rsaMessageBox = TextBox(260, 280, 180, 40, font, 22, MAX_INPUT_DIGITS);

    dhPBox = TextBox(260, 135, 180, 38, font, 21, MAX_INPUT_DIGITS);
    dhGBox = TextBox(260, 200, 180, 38, font, 21, MAX_INPUT_DIGITS);
    dhABox = TextBox(260, 265, 180, 38, font, 21, MAX_INPUT_DIGITS);
    dhBBox = TextBox(260, 330, 180, 38, font, 21, MAX_INPUT_DIGITS);

    gcdABox = TextBox(420, 142, 105, 36, font, 20, MAX_INPUT_DIGITS);
    gcdBBox = TextBox(560, 142, 105, 36, font, 20, MAX_INPUT_DIGITS);

    inverseABox = TextBox(420, 242, 105, 36, font, 20, MAX_INPUT_DIGITS);
    inverseModBox = TextBox(700, 242, 105, 36, font, 20, MAX_INPUT_DIGITS);

    powerBaseBox = TextBox(420, 342, 105, 36, font, 20, MAX_INPUT_DIGITS);
    powerExponentBox = TextBox(560, 342, 105, 36, font, 20, MAX_INPUT_DIGITS);
    powerModBox = TextBox(700, 342, 105, 36, font, 20, MAX_INPUT_DIGITS);

    rsaBenchmarkRunsBox = TextBox(670, 232, 95, 22, font, 16, MAX_INPUT_DIGITS);
    sieveBenchmarkMaxBox = TextBox(625, 322, 95, 22, font, 16, MAX_INPUT_DIGITS);

    primeInputBox.setText("17");

    rsaMinPrimeBox.setText("50");
    rsaMaxPrimeBox.setText("100");
    rsaMessageBox.setText("42");

    dhPBox.setText("23");
    dhGBox.setText("5");
    dhABox.setText("6");
    dhBBox.setText("15");

    gcdABox.setText("48");
    gcdBBox.setText("18");

    inverseABox.setText("3");
    inverseModBox.setText("616");

    powerBaseBox.setText("42");
    powerExponentBox.setText("3");
    powerModBox.setText("667");

    rsaBenchmarkRunsBox.setText("60000");
    sieveBenchmarkMaxBox.setText("5000000");

    primeInputBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    rsaMinPrimeBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    rsaMaxPrimeBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    rsaMessageBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    dhPBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    dhGBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    dhABox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    dhBBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    gcdABox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    gcdBBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    inverseABox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    inverseModBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    powerBaseBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    powerExponentBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    powerModBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    rsaBenchmarkRunsBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
    sieveBenchmarkMaxBox.setColors(COLOR_INPUT, COLOR_LINE, COLOR_TEXT);
}


// Clears calculated output when switching screens or starting a new result.
void App::clearOutput()
{
    lastResultText = "";
    rsaOutputReady = false;
    dhOutputReady = false;
    rsaKeyColumn = "";
    rsaEncryptColumn = "";
    rsaDecryptColumn = "";
    dhSetupColumn = "";
    dhAliceColumn = "";
    dhBobColumn = "";
}

// Polls SFML events and routes them to mouse or keyboard handlers.
void App::processEvents()
{
    while(true)
    {
        optional<sf::Event> event = window.pollEvent();

        if(!event.has_value())
        {
            break;
        }

        if(event->is<sf::Event::Closed>())
        {
            window.close();
        }

        handleMouseClick(*event);
        handleTextInput(*event);
    }
}

// Handles all button clicks and textbox focus changes for the active screen.
void App::handleMouseClick(const sf::Event& event)
{
    if(homeButton.isClicked(event))
    {
        currentScreen = HOME_SCREEN;
        clearOutput();
    }
    else if(primeButton.isClicked(event))
    {
        currentScreen = PRIME_SCREEN;
        clearOutput();
    }
    else if(rsaButton.isClicked(event))
    {
        currentScreen = RSA_SCREEN;
        clearOutput();
    }
    else if(dhButton.isClicked(event))
    {
        currentScreen = DH_SCREEN;
        clearOutput();
    }
    else if(keyToolsButton.isClicked(event))
    {
        currentScreen = KEY_TOOLS_SCREEN;
        clearOutput();
    }
    else if(benchmarkButton.isClicked(event))
    {
        currentScreen = BENCHMARK_SCREEN;
        clearOutput();
    }
    else if(saveButton.isClicked(event))
    {
        saveLastResultToFile();
    }
    else if(exitButton.isClicked(event))
    {
        window.close();
    }

    primeInputBox.handleMouseClick(event);

    rsaMinPrimeBox.handleMouseClick(event);
    rsaMaxPrimeBox.handleMouseClick(event);
    rsaMessageBox.handleMouseClick(event);

    dhPBox.handleMouseClick(event);
    dhGBox.handleMouseClick(event);
    dhABox.handleMouseClick(event);
    dhBBox.handleMouseClick(event);

    gcdABox.handleMouseClick(event);
    gcdBBox.handleMouseClick(event);
    inverseABox.handleMouseClick(event);
    inverseModBox.handleMouseClick(event);
    powerBaseBox.handleMouseClick(event);
    powerExponentBox.handleMouseClick(event);
    powerModBox.handleMouseClick(event);

    rsaBenchmarkRunsBox.handleMouseClick(event);
    sieveBenchmarkMaxBox.handleMouseClick(event);

    if(currentScreen == PRIME_SCREEN && primeRunButton.isClicked(event))
    {
        runPrimeTestFromScreen();
    }

    if(currentScreen == RSA_SCREEN && rsaRunButton.isClicked(event))
    {
        runRSAFromScreen();
    }

    if(currentScreen == DH_SCREEN && dhRunButton.isClicked(event))
    {
        runDHFromScreen();
    }

    if(currentScreen == KEY_TOOLS_SCREEN && gcdToolButton.isClicked(event))
    {
        runGCDToolFromScreen();
    }

    if(currentScreen == KEY_TOOLS_SCREEN && inverseToolButton.isClicked(event))
    {
        runInverseToolFromScreen();
    }

    if(currentScreen == KEY_TOOLS_SCREEN && modExpToolButton.isClicked(event))
    {
        runModExpToolFromScreen();
    }

    if(currentScreen == BENCHMARK_SCREEN && rsaBenchmarkButton.isClicked(event))
    {
        runRSABenchmarkFromScreen();
    }

    if(currentScreen == BENCHMARK_SCREEN && sieveBenchmarkButton.isClicked(event))
    {
        runSieveBenchmarkFromScreen();
    }
}

// Sends numeric keyboard input to the text boxes on the active screen.
void App::handleTextInput(const sf::Event& event)
{
    primeInputBox.handleTextInput(event);

    rsaMinPrimeBox.handleTextInput(event);
    rsaMaxPrimeBox.handleTextInput(event);
    rsaMessageBox.handleTextInput(event);

    dhPBox.handleTextInput(event);
    dhGBox.handleTextInput(event);
    dhABox.handleTextInput(event);
    dhBBox.handleTextInput(event);

    gcdABox.handleTextInput(event);
    gcdBBox.handleTextInput(event);
    inverseABox.handleTextInput(event);
    inverseModBox.handleTextInput(event);
    powerBaseBox.handleTextInput(event);
    powerExponentBox.handleTextInput(event);
    powerModBox.handleTextInput(event);
    rsaBenchmarkRunsBox.handleTextInput(event);
    sieveBenchmarkMaxBox.handleTextInput(event);
}

// Reserved for future per-frame state updates; current behavior is event-driven.
void App::update()
{
}

// Clears the window and draws the sidebar, screen content, and result area.
void App::render()
{
    window.clear(COLOR_APP_BG);

    drawSidebar();
    drawMainArea();

    if(currentScreen != HOME_SCREEN)
    {
        drawOutputPanel();
    }

    window.display();
}

// Draws the persistent left navigation panel and screen buttons.
void App::drawSidebar()
{
    sf::RectangleShape sidebar;
    sidebar.setPosition(sf::Vector2f(0.0f, 0.0f));
    sidebar.setSize(sf::Vector2f(220.0f, 750.0f));
    sidebar.setFillColor(COLOR_SIDEBAR);

    window.draw(sidebar);

    homeButton.draw(window);
    primeButton.draw(window);
    rsaButton.draw(window);
    dhButton.draw(window);
    keyToolsButton.draw(window);
    benchmarkButton.draw(window);
    saveButton.draw(window);
    exitButton.draw(window);
}

// Draws the active screen and output panel when the screen uses one.
void App::drawMainArea()
{
    sf::RectangleShape mainArea;
    mainArea.setPosition(sf::Vector2f(230.0f, 20.0f));

    if(currentScreen == HOME_SCREEN)
    {
        mainArea.setSize(sf::Vector2f(950.0f, 710.0f));
    }
    else
    {
        mainArea.setSize(sf::Vector2f(950.0f, 380.0f));
    }
    mainArea.setFillColor(COLOR_MAIN);
    mainArea.setOutlineColor(COLOR_LINE);
    mainArea.setOutlineThickness(2.0f);

    window.draw(mainArea);

    if(currentScreen == HOME_SCREEN)
    {
        drawHomeScreen();
    }
    else if(currentScreen == PRIME_SCREEN)
    {
        drawPrimeScreen();
    }
    else if(currentScreen == RSA_SCREEN)
    {
        drawRSAScreen();
    }
    else if(currentScreen == DH_SCREEN)
    {
        drawDHScreen();
    }
    else if(currentScreen == KEY_TOOLS_SCREEN)
    {
        drawKeyToolsScreen();
    }
    else if(currentScreen == BENCHMARK_SCREEN)
    {
        drawBenchmarkScreen();
    }
}

// Draws the result panel for calculation screens and benchmarks.
void App::drawOutputPanel()
{
    sf::RectangleShape outputPanel;
    outputPanel.setPosition(sf::Vector2f(230.0f, 420.0f));
    outputPanel.setSize(sf::Vector2f(950.0f, 310.0f));
    outputPanel.setFillColor(COLOR_OUTPUT);
    outputPanel.setOutlineColor(COLOR_LINE);
    outputPanel.setOutlineThickness(2.0f);

    window.draw(outputPanel);

    drawText("Output", 250.0f, 430.0f, 26, COLOR_TEXT);

    if(currentScreen == RSA_SCREEN && rsaOutputReady)
    {
        drawRSAOutputColumns();
    }
    else if(currentScreen == DH_SCREEN && dhOutputReady)
    {
        drawDHOutputColumns();
    }
    else if(currentScreen == PRIME_SCREEN && !lastResultText.empty())
    {
        drawWrappedTextWithFont(monoFont, lastResultText, 250.0f, 470.0f, 22, COLOR_TEXT, 78, 9);
    }
    else if(currentScreen == BENCHMARK_SCREEN && !lastResultText.empty())
    {
        drawPlainTextWithFont(monoFont, lastResultText, 250.0f, 465.0f, 16, COLOR_TEXT, 15);
    }
    else
    {
        drawWrappedTextWithFont(monoFont, lastResultText, 250.0f, 465.0f, 18, COLOR_TEXT, 94, 12);
    }
}

// Draws the home dashboard cards without the output panel.
void App::drawHomeScreen()
{
    drawTextWithFont(boldFont, "CryptoCalc", 260.0f, 42.0f, 42, COLOR_TEXT);
    drawText("Prime checks, RSA, Diffie-Hellman, modular arithmetic, benchmarks, and exports.", 260.0f, 90.0f, 23, COLOR_MUTED);

    sf::RectangleShape accentLine;
    accentLine.setPosition(sf::Vector2f(260.0f, 130.0f));
    accentLine.setSize(sf::Vector2f(860.0f, 2.0f));
    accentLine.setFillColor(COLOR_CYAN);
    window.draw(accentLine);

    sf::RectangleShape summaryBox;
    summaryBox.setPosition(sf::Vector2f(260.0f, 158.0f));
    summaryBox.setSize(sf::Vector2f(860.0f, 72.0f));
    summaryBox.setFillColor(COLOR_CARD);
    summaryBox.setOutlineColor(COLOR_LINE);
    summaryBox.setOutlineThickness(1.0f);
    window.draw(summaryBox);

    drawText("Click a tab from the sidebar, run a calculation, then save the current output if needed", 280.0f, 180.0f, 22, COLOR_MUTED);

    float cardW = 270.0f;
    float cardH = 120.0f;
    float x1 = 260.0f;
    float x2 = 555.0f;
    float x3 = 850.0f;
    float y1 = 265.0f;
    float y2 = 420.0f;

    sf::RectangleShape card;
    card.setSize(sf::Vector2f(cardW, cardH));
    card.setFillColor(COLOR_CARD);
    card.setOutlineColor(COLOR_LINE);
    card.setOutlineThickness(1.0f);

    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(cardW, 4.0f));

    card.setPosition(sf::Vector2f(x1, y1));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x1, y1));
    bar.setFillColor(COLOR_CYAN);
    window.draw(bar);
    drawText("PRIME", x1 + 18.0f, y1 + 18.0f, 14, COLOR_CYAN);
    drawText("Primality Test", x1 + 18.0f, y1 + 39.0f, 23, COLOR_TEXT);
    drawText("Check a number and get a", x1 + 18.0f, y1 + 70.0f, 16, COLOR_MUTED);
    drawText("divisor when it is composite.", x1 + 18.0f, y1 + 90.0f, 16, COLOR_MUTED);

    card.setPosition(sf::Vector2f(x2, y1));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x2, y1));
    bar.setFillColor(COLOR_PURPLE);
    window.draw(bar);
    drawText("RSA", x2 + 18.0f, y1 + 18.0f, 14, COLOR_PURPLE);
    drawText("RSA", x2 + 18.0f, y1 + 39.0f, 23, COLOR_TEXT);
    drawText("Generate keys and check", x2 + 18.0f, y1 + 70.0f, 16, COLOR_MUTED);
    drawText("numeric encrypt/decrypt cycles.", x2 + 18.0f, y1 + 90.0f, 16, COLOR_MUTED);

    card.setPosition(sf::Vector2f(x3, y1));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x3, y1));
    bar.setFillColor(COLOR_GREEN);
    window.draw(bar);
    drawText("DH", x3 + 18.0f, y1 + 18.0f, 14, COLOR_GREEN);
    drawText("Diffie-Hellman", x3 + 18.0f, y1 + 39.0f, 23, COLOR_TEXT);
    drawText("Compute matching shared keys", x3 + 18.0f, y1 + 70.0f, 16, COLOR_MUTED);
    drawText("from public and private values.", x3 + 18.0f, y1 + 90.0f, 16, COLOR_MUTED);

    card.setPosition(sf::Vector2f(x1, y2));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x1, y2));
    bar.setFillColor(COLOR_AMBER);
    window.draw(bar);
    drawText("MOD", x1 + 18.0f, y2 + 18.0f, 14, COLOR_AMBER);
    drawText("Math Tools", x1 + 18.0f, y2 + 39.0f, 23, COLOR_TEXT);
    drawText("Calculate GCD, modular", x1 + 18.0f, y2 + 70.0f, 16, COLOR_MUTED);
    drawText("inverse, and power mod.", x1 + 18.0f, y2 + 90.0f, 16, COLOR_MUTED);

    card.setPosition(sf::Vector2f(x2, y2));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x2, y2));
    bar.setFillColor(COLOR_BLUE);
    window.draw(bar);
    drawText("TIME", x2 + 18.0f, y2 + 18.0f, 14, COLOR_BLUE);
    drawText("Benchmarks", x2 + 18.0f, y2 + 39.0f, 23, COLOR_TEXT);
    drawText("Run longer timing tests", x2 + 18.0f, y2 + 70.0f, 16, COLOR_MUTED);
    drawText("for RSA and prime sieving.", x2 + 18.0f, y2 + 90.0f, 16, COLOR_MUTED);

    card.setPosition(sf::Vector2f(x3, y2));
    window.draw(card);
    bar.setPosition(sf::Vector2f(x3, y2));
    bar.setFillColor(COLOR_LINE);
    window.draw(bar);
    drawText("SAVE", x3 + 18.0f, y2 + 18.0f, 14, COLOR_LINE);
    drawText("Save Result", x3 + 18.0f, y2 + 39.0f, 23, COLOR_TEXT);
    drawText("Export the current output", x3 + 18.0f, y2 + 70.0f, 16, COLOR_MUTED);
    drawText("to a timestamped text file.", x3 + 18.0f, y2 + 90.0f, 16, COLOR_MUTED);

    sf::RectangleShape bottomBox;
    bottomBox.setPosition(sf::Vector2f(260.0f, 600.0f));
    bottomBox.setSize(sf::Vector2f(860.0f, 78.0f));
    bottomBox.setFillColor(COLOR_CARD);
    bottomBox.setOutlineColor(COLOR_LINE);
    bottomBox.setOutlineThickness(1.0f);
    window.draw(bottomBox);

    drawText("Output", 285.0f, 605.0f, 23, COLOR_TEXT);
    drawText("Results appear in the lower output panel on every tool page except the Home page.", 285.0f, 635.0f, 22, COLOR_MUTED);
}

// Draws the primality checker input and explanation card.
void App::drawPrimeScreen()
{
    drawText("Primality Test", 260.0f, 50.0f, 32, COLOR_TEXT);
    drawText("Enter a positive whole number to checks whether it is prime.", 260.0f, 88.0f, 18, COLOR_MUTED);
    drawText("Enter a number:", 260.0f, 122.0f, 21, COLOR_TEXT);

    primeInputBox.draw(window);
    primeRunButton.draw(window);

    sf::RectangleShape definitionBox;
    definitionBox.setPosition(sf::Vector2f(535.0f, 140.0f));
    definitionBox.setSize(sf::Vector2f(585.0f, 210.0f));
    definitionBox.setFillColor(COLOR_CARD);
    definitionBox.setOutlineColor(COLOR_CYAN);
    definitionBox.setOutlineThickness(1.0f);
    window.draw(definitionBox);

    drawText("Prime number", 560.0f, 145.0f, 25, COLOR_TEXT);
    drawText("A prime number is greater than 1 and has exactly", 560.0f, 182.0f, 18, COLOR_MUTED);
    drawText("two positive factors: 1 and itself.", 560.0f, 208.0f, 18, COLOR_MUTED);
    drawText("Algorithm: Sieve of Eratosthenes", 560.0f, 250.0f, 20, COLOR_AMBER);
    drawText("The app sieves primes up to sqrt(n), then checks", 560.0f, 280.0f, 17, COLOR_MUTED);
    drawText("whether any of those primes divide n evenly.", 560.0f, 304.0f, 17, COLOR_MUTED);
}

// Draws the RSA input controls, formula cards, and variable reference cards.
void App::drawRSAScreen()
{
    drawText("RSA", 260.0f, 45.0f, 32, COLOR_TEXT);
    drawText("Enter a prime range and a numeric message. The message must be smaller than n.", 260.0f, 82.0f, 17, COLOR_MUTED);

    drawText("Min prime:", 260.0f, 110.0f, 20, COLOR_TEXT);
    drawText("Max prime:", 260.0f, 180.0f, 20, COLOR_TEXT);
    drawText("Message:", 260.0f, 250.0f, 20, COLOR_TEXT);

    rsaMinPrimeBox.draw(window);
    rsaMaxPrimeBox.draw(window);
    rsaMessageBox.draw(window);
    rsaRunButton.draw(window);

    sf::RectangleShape inputBox;
    inputBox.setPosition(sf::Vector2f(500.0f, 125.0f));
    inputBox.setSize(sf::Vector2f(265.0f, 150.0f));
    inputBox.setFillColor(COLOR_CARD);
    inputBox.setOutlineColor(COLOR_CYAN);
    inputBox.setOutlineThickness(1.0f);
    window.draw(inputBox);
    drawText("Inputs", 520.0f, 140.0f, 22, COLOR_TEXT);
    drawText("Min/Max prime: range", 520.0f, 175.0f, 15, COLOR_MUTED);
    drawText("used to choose p and q.", 520.0f, 197.0f, 15, COLOR_MUTED);
    drawText("Message m: number to", 520.0f, 228.0f, 15, COLOR_MUTED);
    drawText("encrypt. It must be < n.", 520.0f, 250.0f, 15, COLOR_MUTED);

    sf::RectangleShape keyBox;
    keyBox.setPosition(sf::Vector2f(790.0f, 125.0f));
    keyBox.setSize(sf::Vector2f(340.0f, 150.0f));
    keyBox.setFillColor(sf::Color(236, 253, 245));
    keyBox.setOutlineColor(COLOR_GREEN);
    keyBox.setOutlineThickness(1.0f);
    window.draw(keyBox);
    drawText("Keys", 810.0f, 140.0f, 22, COLOR_TEXT);
    drawText("p and q are the two primes.", 810.0f, 174.0f, 15, COLOR_MUTED);
    drawText("n = p * q is the modulus.", 810.0f, 196.0f, 15, COLOR_MUTED);
    drawText("(e, n) means public key: e with n.", 810.0f, 228.0f, 15, COLOR_MUTED);
    drawText("(d, n) means private key: d with n.", 810.0f, 250.0f, 15, COLOR_MUTED);

    drawRSAFormula(610.0f, 315.0f);
}

// Draws Diffie-Hellman inputs and reference cards for public/private values.
void App::drawDHScreen()
{
    drawText("Diffie-Hellman", 260.0f, 45.0f, 32, COLOR_TEXT);
    drawText("Enter public setup values p and g, then Alice and Bob private secrets.", 260.0f, 82.0f, 17, COLOR_MUTED);

    drawText("p:", 260.0f, 105.0f, 20, COLOR_TEXT);
    drawText("g:", 260.0f, 170.0f, 20, COLOR_TEXT);
    drawText("Alice secret a:", 260.0f, 240.0f, 20, COLOR_TEXT);
    drawText("Bob secret b:", 260.0f, 305.0f, 20, COLOR_TEXT);

    dhPBox.draw(window);
    dhGBox.draw(window);
    dhABox.draw(window);
    dhBBox.draw(window);
    dhRunButton.draw(window);

    sf::RectangleShape publicBox;
    publicBox.setPosition(sf::Vector2f(500.0f, 125.0f));
    publicBox.setSize(sf::Vector2f(270.0f, 185.0f));
    publicBox.setFillColor(COLOR_CARD);
    publicBox.setOutlineColor(COLOR_CYAN);
    publicBox.setOutlineThickness(1.0f);
    window.draw(publicBox);
    drawText("Public values", 520.0f, 142.0f, 22, COLOR_TEXT);
    drawText("p: prime modulus. It is the", 520.0f, 178.0f, 15, COLOR_MUTED);
    drawText("prime number used for mod.", 520.0f, 200.0f, 15, COLOR_MUTED);
    drawText("g: public base. It is the", 520.0f, 230.0f, 15, COLOR_MUTED);
    drawText("starting value for powers.", 520.0f, 252.0f, 15, COLOR_MUTED);
    drawText("p, g, A, and B are shared.", 520.0f, 282.0f, 15, COLOR_MUTED);

    sf::RectangleShape aliceBox;
    aliceBox.setPosition(sf::Vector2f(795.0f, 125.0f));
    aliceBox.setSize(sf::Vector2f(165.0f, 185.0f));
    aliceBox.setFillColor(sf::Color(236, 253, 245));
    aliceBox.setOutlineColor(COLOR_GREEN);
    aliceBox.setOutlineThickness(1.0f);
    window.draw(aliceBox);
    drawText("Alice", 845.0f, 142.0f, 23, COLOR_TEXT);
    drawCenteredText("a is private", 795.0f, 180.0f, 165.0f, 15, COLOR_TEXT);
    drawText("A = g^a mod p", 815.0f, 220.0f, 15, COLOR_MUTED);
    drawText("key = B^a mod p", 810.0f, 255.0f, 14, COLOR_MUTED);

    sf::RectangleShape bobBox;
    bobBox.setPosition(sf::Vector2f(985.0f, 125.0f));
    bobBox.setSize(sf::Vector2f(165.0f, 185.0f));
    bobBox.setFillColor(sf::Color(245, 243, 255));
    bobBox.setOutlineColor(COLOR_PURPLE);
    bobBox.setOutlineThickness(1.0f);
    window.draw(bobBox);
    drawText("Bob", 1038.0f, 142.0f, 23, COLOR_TEXT);
    drawCenteredText("b is private", 985.0f, 180.0f, 165.0f, 15, COLOR_TEXT);
    drawText("B = g^b mod p", 1005.0f, 220.0f, 15, COLOR_MUTED);
    drawText("key = A^b mod p", 1000.0f, 255.0f, 14, COLOR_MUTED);

    drawText("A and B are public results while a and b are never shared", 700.0f, 340.0f, 18, COLOR_TEXT);
}

// Draws the separate GCD, inverse, and power-mod calculator sections.
void App::drawKeyToolsScreen()
{
    drawText("Math Tools", 260.0f, 40.0f, 32, COLOR_TEXT);
    drawText("Each calculator has its own inputs and equation.", 260.0f, 78.0f, 18, COLOR_MUTED);

    sf::RectangleShape gcdSection;
    gcdSection.setPosition(sf::Vector2f(255.0f, 104.0f));
    gcdSection.setSize(sf::Vector2f(880.0f, 88.0f));
    gcdSection.setFillColor(COLOR_CARD);
    gcdSection.setOutlineColor(COLOR_PURPLE);
    gcdSection.setOutlineThickness(1.0f);
    window.draw(gcdSection);

    drawText("GCD", 275.0f, 122.0f, 22, COLOR_TEXT);
    drawTextWithFont(monoFont, "gcd(a, b)", 275.0f, 152.0f, 17, COLOR_MUTED);
    drawText("a", 428.0f, 118.0f, 17, COLOR_TEXT);
    drawText("b", 568.0f, 118.0f, 17, COLOR_TEXT);
    gcdABox.draw(window);
    gcdBBox.draw(window);
    gcdToolButton.draw(window);

    sf::RectangleShape inverseSection;
    inverseSection.setPosition(sf::Vector2f(255.0f, 204.0f));
    inverseSection.setSize(sf::Vector2f(880.0f, 88.0f));
    inverseSection.setFillColor(COLOR_CARD);
    inverseSection.setOutlineColor(COLOR_PURPLE);
    inverseSection.setOutlineThickness(1.0f);
    window.draw(inverseSection);

    drawText("Mod Inverse", 275.0f, 222.0f, 22, COLOR_TEXT);
    drawTextWithFont(monoFont, "a^(-1) mod m", 275.0f, 252.0f, 17, COLOR_MUTED);
    drawText("a", 428.0f, 218.0f, 17, COLOR_TEXT);
    drawText("m", 708.0f, 218.0f, 17, COLOR_TEXT);
    inverseABox.draw(window);
    inverseModBox.draw(window);
    inverseToolButton.draw(window);

    sf::RectangleShape powerSection;
    powerSection.setPosition(sf::Vector2f(255.0f, 304.0f));
    powerSection.setSize(sf::Vector2f(880.0f, 88.0f));
    powerSection.setFillColor(COLOR_CARD);
    powerSection.setOutlineColor(COLOR_PURPLE);
    powerSection.setOutlineThickness(1.0f);
    window.draw(powerSection);

    drawText("Power Mod", 275.0f, 322.0f, 22, COLOR_TEXT);
    drawTextWithFont(monoFont, "base^exp mod m", 275.0f, 352.0f, 17, COLOR_MUTED);
    drawText("base", 428.0f, 318.0f, 17, COLOR_TEXT);
    drawText("exp", 568.0f, 318.0f, 17, COLOR_TEXT);
    drawText("m", 708.0f, 318.0f, 17, COLOR_TEXT);
    powerBaseBox.draw(window);
    powerExponentBox.draw(window);
    powerModBox.draw(window);
    modExpToolButton.draw(window);
}

// Draws the benchmark controls with bounded user inputs.
void App::drawBenchmarkScreen()
{
    drawText("Benchmarks", 260.0f, 50.0f, 32, COLOR_TEXT);
    drawText("Run longer timing tests for RSA and Prime calculations.", 260.0f, 92.0f, 18, COLOR_MUTED);
    drawText("You can adjust the workload, but with restriction limits so it does not run forever.", 260.0f, 118.0f, 18, COLOR_MUTED);

    rsaBenchmarkButton.draw(window);
    sieveBenchmarkButton.draw(window);

    drawText("RSA Benchmark", 540.0f, 180.0f, 22, COLOR_TEXT);
    drawText("Repeats full RSA cycles: key generation, encryption, and decryption.", 540.0f, 207.0f, 17, COLOR_MUTED);
    drawText("Runs per range:", 540.0f, 232.0f, 17, COLOR_TEXT);
    rsaBenchmarkRunsBox.draw(window);
    drawText("allowed: 1000 to 200000", 780.0f, 235.0f, 15, COLOR_MUTED);

    drawText("Sieve Benchmark", 540.0f, 270.0f, 22, COLOR_TEXT);
    drawText("Builds prime tables up to larger limits using the Sieve of Eratosthenes.", 540.0f, 297.0f, 17, COLOR_MUTED);
    drawText("Max limit:", 540.0f, 322.0f, 17, COLOR_TEXT);
    sieveBenchmarkMaxBox.draw(window);
    drawText("allowed: 10000 to 10000000", 735.0f, 325.0f, 15, COLOR_MUTED);

    sf::RectangleShape noteBox;
    noteBox.setPosition(sf::Vector2f(260.0f, 365.0f));
    noteBox.setSize(sf::Vector2f(860.0f, 28.0f));
    noteBox.setFillColor(COLOR_CARD);
    noteBox.setOutlineColor(COLOR_AMBER);
    noteBox.setOutlineThickness(1.0f);
    window.draw(noteBox);
    drawText("Loading text appears before the calculation starts. These tests may take a few seconds. ", 285.0f, 370.0f, 16, COLOR_TEXT);
}

// Convenience wrapper for drawing text with the main UI font.
void App::drawText(const string& text, float x, float y, unsigned int size, const sf::Color& color)
{
    drawTextWithFont(font, text, x, y, size, color);
}

// Draws text with a specific font for UI or monospaced output.
void App::drawTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color)
{
    sf::Text displayText(usedFont, text, size);
    displayText.setFillColor(color);
    displayText.setPosition(sf::Vector2f(x, y));

    window.draw(displayText);
}

// Centers a short label horizontally within a given box width.
void App::drawCenteredText(const string& text, float boxX, float y, float boxWidth, unsigned int size, const sf::Color& color)
{
    sf::Text displayText(font, text, size);
    sf::FloatRect bounds;
    float textX;

    displayText.setFillColor(color);
    bounds = displayText.getLocalBounds();
    textX = boxX + (boxWidth - bounds.size.x) / 2.0f - bounds.position.x;
    displayText.setPosition(sf::Vector2f(textX, y));

    window.draw(displayText);
}

// Wraps long UI text using the main font and a character-based width limit.
void App::drawWrappedText(const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxChars, int maxLines)
{
    drawWrappedTextWithFont(font, text, x, y, size, color, maxChars, maxLines);
}

// Wraps long text without relying on dynamic layout measurements.
void App::drawWrappedTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxChars, int maxLines)
{
    stringstream input(text);
    string originalLine;
    string word;
    string line;
    int linesDrawn;
    float lineSpacing;

    linesDrawn = 0;
    lineSpacing = static_cast<float>(size) + 5.0f;

    while(getline(input, originalLine) && linesDrawn < maxLines)
    {
        stringstream words(originalLine);
        line = "";

        while(words >> word)
        {
            if(line.empty())
            {
                line = word;
            }
            else if(static_cast<int>(line.size() + 1 + word.size()) <= maxChars)
            {
                line += " " + word;
            }
            else
            {
                drawTextWithFont(usedFont, line, x, y + linesDrawn * lineSpacing, size, color);
                linesDrawn++;

                if(linesDrawn >= maxLines)
                {
                    return;
                }

                line = word;
            }
        }

        if(!line.empty() && linesDrawn < maxLines)
        {
            drawTextWithFont(usedFont, line, x, y + linesDrawn * lineSpacing, size, color);
            linesDrawn++;
        }
        else if(originalLine.empty() && linesDrawn < maxLines)
        {
            linesDrawn++;
        }
    }
}



// Draws monospaced output line-by-line so benchmark tables stay aligned.
void App::drawPlainTextWithFont(const sf::Font& usedFont, const string& text, float x, float y, unsigned int size, const sf::Color& color, int maxLines)
{
    stringstream input(text);
    string line;
    int linesDrawn;
    float lineSpacing;

    linesDrawn = 0;
    lineSpacing = static_cast<float>(size) + 4.0f;

    while(getline(input, line) && linesDrawn < maxLines)
    {
        drawTextWithFont(usedFont, line, x, y + linesDrawn * lineSpacing, size, color);
        linesDrawn++;
    }
}

// Draws RSA setup, encryption, and decryption output in three columns.
void App::drawRSAOutputColumns()
{
    sf::RectangleShape keyPanel;
    sf::RectangleShape encryptPanel;
    sf::RectangleShape decryptPanel;

    keyPanel.setPosition(sf::Vector2f(250.0f, 465.0f));
    keyPanel.setSize(sf::Vector2f(285.0f, 245.0f));
    keyPanel.setFillColor(COLOR_CARD);
    keyPanel.setOutlineColor(COLOR_BLUE);
    keyPanel.setOutlineThickness(1.0f);
    window.draw(keyPanel);

    encryptPanel.setPosition(sf::Vector2f(560.0f, 465.0f));
    encryptPanel.setSize(sf::Vector2f(285.0f, 245.0f));
    encryptPanel.setFillColor(sf::Color(236, 253, 245));
    encryptPanel.setOutlineColor(COLOR_GREEN);
    encryptPanel.setOutlineThickness(1.0f);
    window.draw(encryptPanel);

    decryptPanel.setPosition(sf::Vector2f(870.0f, 465.0f));
    decryptPanel.setSize(sf::Vector2f(285.0f, 245.0f));
    decryptPanel.setFillColor(sf::Color(254, 242, 242));
    decryptPanel.setOutlineColor(COLOR_ROSE);
    decryptPanel.setOutlineThickness(1.0f);
    window.draw(decryptPanel);

    drawText("RSA", 265.0f, 475.0f, 21, COLOR_TEXT);
    drawText("Encrypt", 575.0f, 475.0f, 21, COLOR_TEXT);
    drawText("Decrypt", 885.0f, 475.0f, 21, COLOR_TEXT);

    drawWrappedTextWithFont(monoFont, rsaKeyColumn, 265.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
    drawWrappedTextWithFont(monoFont, rsaEncryptColumn, 575.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
    drawWrappedTextWithFont(monoFont, rsaDecryptColumn, 885.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
}

// Draws Diffie-Hellman setup, Alice, and Bob output in three columns.
void App::drawDHOutputColumns()
{
    sf::RectangleShape setupPanel;
    sf::RectangleShape alicePanel;
    sf::RectangleShape bobPanel;

    setupPanel.setPosition(sf::Vector2f(250.0f, 465.0f));
    setupPanel.setSize(sf::Vector2f(285.0f, 245.0f));
    setupPanel.setFillColor(COLOR_CARD);
    setupPanel.setOutlineColor(COLOR_CYAN);
    setupPanel.setOutlineThickness(1.0f);
    window.draw(setupPanel);

    alicePanel.setPosition(sf::Vector2f(560.0f, 465.0f));
    alicePanel.setSize(sf::Vector2f(285.0f, 245.0f));
    alicePanel.setFillColor(sf::Color(236, 253, 245));
    alicePanel.setOutlineColor(COLOR_BLUE);
    alicePanel.setOutlineThickness(1.0f);
    window.draw(alicePanel);

    bobPanel.setPosition(sf::Vector2f(870.0f, 465.0f));
    bobPanel.setSize(sf::Vector2f(285.0f, 245.0f));
    bobPanel.setFillColor(sf::Color(245, 243, 255));
    bobPanel.setOutlineColor(COLOR_PURPLE);
    bobPanel.setOutlineThickness(1.0f);
    window.draw(bobPanel);

    drawText("Setup", 265.0f, 475.0f, 21, COLOR_TEXT);
    drawText("Alice", 575.0f, 475.0f, 21, COLOR_TEXT);
    drawText("Bob", 885.0f, 475.0f, 21, COLOR_TEXT);

    drawWrappedTextWithFont(monoFont, dhSetupColumn, 265.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
    drawWrappedTextWithFont(monoFont, dhAliceColumn, 575.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
    drawWrappedTextWithFont(monoFont, dhBobColumn, 885.0f, 510.0f, 15, COLOR_TEXT, 28, 10);
}

// Draws RSA formulas with visually raised exponents instead of Unicode superscripts.
void App::drawRSAFormula(float x, float y)
{
    drawText("Encrypt:", x - 120.0f, y, 22, COLOR_TEXT);
    drawText("c = m", x, y, 24, COLOR_TEXT);
    drawText("e", x + 63.0f, y - 11.0f, 15, COLOR_TEXT);
    drawText(" mod n", x + 78.0f, y, 24, COLOR_TEXT);

    drawText("Decrypt:", x + 205.0f, y, 22, COLOR_TEXT);
    drawText("m = c", x + 320.0f, y, 24, COLOR_TEXT);
    drawText("d", x + 383.0f, y - 11.0f, 15, COLOR_TEXT);
    drawText(" mod n", x + 398.0f, y, 24, COLOR_TEXT);
}

// Validates the primality input and reports either prime status or a divisor.
void App::runPrimeTestFromScreen()
{
    long long number;
    long long divisor;

    if(!stringToLongLong(primeInputBox.getText(), number))
    {
        clearOutput();
        lastResultText = "Input must be a whole number with at most 8 digits.";
        return;
    }

    rsaOutputReady = false;
    dhOutputReady = false;

    divisor = smallestDivisorUsingSieve(number);

    if(divisor == -1)
    {
        lastResultText = to_string(number) + " is prime.";
    }
    else if(number < 2)
    {
        lastResultText = to_string(number) + " is not prime.\n";
        lastResultText += "Prime numbers must be greater than 1.";
    }
    else
    {
        lastResultText = to_string(number) + " is not prime.\n";
        lastResultText += to_string(number) + " is divisible by " + to_string(divisor) + ".";
    }
}

// Validates RSA inputs, runs key generation/encryption/decryption, and formats output.
void App::runRSAFromScreen()
{
    long long minPrime;
    long long maxPrime;
    long long message;
    RSARun run;

    if(!stringToLongLong(rsaMinPrimeBox.getText(), minPrime) ||
        !stringToLongLong(rsaMaxPrimeBox.getText(), maxPrime) ||
        !stringToLongLong(rsaMessageBox.getText(), message))
    {
        clearOutput();
        lastResultText = "Inputs must be whole numbers with at most 8 digits.";
        return;
    }

    if(minPrime > maxPrime)
    {
        clearOutput();
        lastResultText = "Min prime must be less than or equal to max prime.";
        return;
    }

    if(maxPrime - minPrime > MAX_RSA_RANGE_WIDTH)
    {
        clearOutput();
        lastResultText = "RSA prime range is too large for this tool. Use a range of 100000 numbers or fewer.";
        return;
    }

    if(maxPrime > 3037000499LL)
    {
        clearOutput();
        lastResultText = "RSA primes are too large for this tool. Use prime values up to 3037000499.";
        return;
    }

    if(!hasAtLeastTwoOddPrimes(minPrime, maxPrime))
    {
        clearOutput();
        lastResultText = "RSA needs a range with at least two different odd primes.";
        return;
    }

    run = runRSADemo(message, minPrime, maxPrime);

    if(run.keys.n == -1)
    {
        clearOutput();
        lastResultText = "RSA needs a range with at least two different odd primes.";
        return;
    }

    if(message >= run.keys.n)
    {
        clearOutput();
        lastResultText = "Message must be less than n = " + to_string(run.keys.n);
        return;
    }

    rsaOutputReady = true;

    rsaKeyColumn = "Primes:\n";
    rsaKeyColumn += "p = " + to_string(run.keys.p) + "\n";
    rsaKeyColumn += "q = " + to_string(run.keys.q) + "\n";
    rsaKeyColumn += "n = p * q = " + to_string(run.keys.n) + "\n";
    rsaKeyColumn += "phi = " + to_string(run.keys.phi) + "\n";
    rsaKeyColumn += "e = " + to_string(run.keys.e) + "\n";
    rsaKeyColumn += "d = " + to_string(run.keys.d) + "\n";
    rsaKeyColumn += "Public: (" + to_string(run.keys.e) + ", " + to_string(run.keys.n) + ")\n";
    rsaKeyColumn += "Private: (" + to_string(run.keys.d) + ", " + to_string(run.keys.n) + ")";

    rsaEncryptColumn = "Message:\n";
    rsaEncryptColumn += "m = " + to_string(run.message) + "\n";
    rsaEncryptColumn += "Use public key\n";
    rsaEncryptColumn += "e = " + to_string(run.keys.e) + "\n";
    rsaEncryptColumn += "n = " + to_string(run.keys.n) + "\n";
    rsaEncryptColumn += "c = m^e mod n\n";
    rsaEncryptColumn += "c = " + to_string(run.message) + "^" + to_string(run.keys.e) + " mod " + to_string(run.keys.n) + "\n";
    rsaEncryptColumn += "Ciphertext:\n";
    rsaEncryptColumn += "c = " + to_string(run.ciphertext);

    rsaDecryptColumn = "Ciphertext:\n";
    rsaDecryptColumn += "c = " + to_string(run.ciphertext) + "\n";
    rsaDecryptColumn += "Use private key\n";
    rsaDecryptColumn += "d = " + to_string(run.keys.d) + "\n";
    rsaDecryptColumn += "n = " + to_string(run.keys.n) + "\n";
    rsaDecryptColumn += "m = c^d mod n\n";
    rsaDecryptColumn += "m = " + to_string(run.ciphertext) + "^" + to_string(run.keys.d) + " mod " + to_string(run.keys.n) + "\n";
    rsaDecryptColumn += "Recovered:\n";
    rsaDecryptColumn += "m = " + to_string(run.decryptedMessage);

    lastResultText = rsaRunToString(run);
}

// Validates Diffie-Hellman inputs and confirms both shared keys match.
void App::runDHFromScreen()
{
    long long p;
    long long g;
    long long a;
    long long b;
    DHRun run;

    if(!stringToLongLong(dhPBox.getText(), p) || !stringToLongLong(dhGBox.getText(), g) || !stringToLongLong(dhABox.getText(), a) || !stringToLongLong(dhBBox.getText(), b))
    {
        clearOutput();
        lastResultText = "Inputs must be whole numbers with at most 8 digits.";
        return;
    }

    if(p <= 1 || !isPrimeSieve(p))
    {
        clearOutput();
        lastResultText = "Diffie-Hellman p must be a prime greater than 1.";
        return;
    }

    if(g <= 0 || a <= 0 || b <= 0)
    {
        clearOutput();
        lastResultText = "Diffie-Hellman g, a, and b must be positive numbers.";
        return;
    }

    run = runDHExchange(p, g, a, b);
    rsaOutputReady = false;
    dhOutputReady = true;

    dhSetupColumn = "Public values:\n";
    dhSetupColumn += "p = " + to_string(run.p) + "\n";
    dhSetupColumn += "g = " + to_string(run.g) + "\n";
    dhSetupColumn += "p is the prime modulus\n";
    dhSetupColumn += "g is the public base\n";
    dhSetupColumn += "a and b are private";

    dhAliceColumn = "Private secret:\n";
    dhAliceColumn += "a = " + to_string(run.a) + "\n";
    dhAliceColumn += "Public value:\n";
    dhAliceColumn += "A = g^a mod p\n";
    dhAliceColumn += "A = " + to_string(run.g) + "^" + to_string(run.a) + " mod " + to_string(run.p) + "\n";
    dhAliceColumn += "A = " + to_string(run.A) + "\n";
    dhAliceColumn += "Shared key:\n";
    dhAliceColumn += "B^a mod p = " + to_string(run.sharedA);

    dhBobColumn = "Private secret:\n";
    dhBobColumn += "b = " + to_string(run.b) + "\n";
    dhBobColumn += "Public value:\n";
    dhBobColumn += "B = g^b mod p\n";
    dhBobColumn += "B = " + to_string(run.g) + "^" + to_string(run.b) + " mod " + to_string(run.p) + "\n";
    dhBobColumn += "B = " + to_string(run.B) + "\n";
    dhBobColumn += "Shared key:\n";
    dhBobColumn += "A^b mod p = " + to_string(run.sharedB);

    lastResultText = dhRunToString(run);
}

// Runs the GCD calculator using its dedicated input boxes.
void App::runGCDToolFromScreen()
{
    long long a;
    long long b;

    if(!stringToLongLong(gcdABox.getText(), a) || !stringToLongLong(gcdBBox.getText(), b))
    {
        clearOutput();
        lastResultText = "A and B must be whole numbers with at most 8 digits.";
        return;
    }

    rsaOutputReady = false;
    dhOutputReady = false;
    lastResultText = "GCD Result\n\n";
    lastResultText += "Expression: gcd(a, b)\n";
    lastResultText += "Substitute: gcd(" + to_string(a) + ", " + to_string(b) + ")\n";
    lastResultText += "Result: gcd(" + to_string(a) + ", " + to_string(b) + ") = ";
    lastResultText += to_string(gcd(a, b));
}

// Runs the modular inverse calculator and reports when no inverse exists.
void App::runInverseToolFromScreen()
{
    long long a;
    long long mod;
    long long inverse;

    if(!stringToLongLong(inverseABox.getText(), a) || !stringToLongLong(inverseModBox.getText(), mod))
    {
        clearOutput();
        lastResultText = "A and Mod must be whole numbers with at most 8 digits.";
        return;
    }

    rsaOutputReady = false;
    dhOutputReady = false;

    if(mod <= 1)
    {
        lastResultText = "Mod must be greater than 1.";
        return;
    }

    inverse = modInverse(a, mod);

    if(inverse == -1)
    {
        lastResultText = "Modular Inverse Result\n\n";
        lastResultText += to_string(a) + "^(-1) mod " + to_string(mod) + " does not exist.\n";
        lastResultText += "This happens when gcd(a, mod) is not 1.";
        return;
    }

    lastResultText = "Modular Inverse Result\n\n";
    lastResultText += "Expression: a^(-1) mod m\n";
    lastResultText += "Substitute: " + to_string(a) + "^(-1) mod " + to_string(mod) + "\n";
    lastResultText += "Result: " + to_string(a) + "^(-1) mod " + to_string(mod) + " = " + to_string(inverse);
}

// Runs the modular exponent calculator with modulus validation.
void App::runModExpToolFromScreen()
{
    long long base;
    long long exponent;
    long long mod;
    long long answer;

    if(!stringToLongLong(powerBaseBox.getText(), base) || !stringToLongLong(powerExponentBox.getText(), exponent) || !stringToLongLong(powerModBox.getText(), mod))
    {
        clearOutput();
        lastResultText = "Base, exp, and mod must be whole numbers with at most 8 digits.";
        return;
    }

    rsaOutputReady = false;
    dhOutputReady = false;

    if(mod <= 0)
    {
        lastResultText = "Mod must be greater than 0.";
        return;
    }

    answer = modExp(base, exponent, mod);

    lastResultText = "Power Mod Result\n\n";
    lastResultText += "Expression: base^exp mod m\n";
    lastResultText += "Substitute: " + to_string(base) + "^" + to_string(exponent) + " mod " + to_string(mod) + "\n";
    lastResultText += "Result: " + to_string(base) + "^" + to_string(exponent) + " mod " + to_string(mod);
    lastResultText += " = " + to_string(answer);
}

// Runs the RSA benchmark with a bounded number of repeated cycles.
void App::runRSABenchmarkFromScreen()
{
    BenchmarkResult result;
    long long runs;

    rsaOutputReady = false;
    dhOutputReady = false;

    if(!stringToLongLong(rsaBenchmarkRunsBox.getText(), runs))
    {
        lastResultText = "RSA Benchmark Error\n\nRuns per range must be a whole number.";
        return;
    }

    if(runs < 1000 || runs > 200000)
    {
        lastResultText = "RSA Benchmark Error\n\nRuns per range must be between 1000 and 200000.";
        return;
    }

    lastResultText = "Loading RSA Benchmark...\n\nRunning " + to_string(runs) + " full RSA cycles per range.\nThis may take a few seconds.";
    render();

    result = benchmarkRSA((int)runs);
    lastResultText = benchmarkResultToString(result);
}

// Runs the sieve benchmark with a bounded maximum limit.
void App::runSieveBenchmarkFromScreen()
{
    BenchmarkResult result;
    long long maxLimit;

    rsaOutputReady = false;
    dhOutputReady = false;

    if(!stringToLongLong(sieveBenchmarkMaxBox.getText(), maxLimit))
    {
        lastResultText = "Sieve Benchmark Error\n\nMax limit must be a whole number.";
        return;
    }

    if(maxLimit < 10000 || maxLimit > 10000000)
    {
        lastResultText = "Sieve Benchmark Error\n\nMax limit must be between 10000 and 10000000.";
        return;
    }

    lastResultText = "Loading Sieve Benchmark...\n\nBuilding prime tables up to " + to_string(maxLimit) + ".\nThis may take a few seconds.";
    render();

    result = benchmarkSieve(maxLimit);
    lastResultText = benchmarkResultToString(result);
}

// Saves the current output panel to a timestamped text file.
void App::saveLastResultToFile()
{
    string fileName;
    string content;

    if(lastResultText.empty())
    {
        rsaOutputReady = false;
        dhOutputReady = false;
        lastResultText = "There is no result to save yet.";
        return;
    }

    fileName = "cryptocalc_result_" + getTimestampString() + ".txt";

    content = "CryptoCalc Result\n";
    content += "Tab: " + currentScreenName() + "\n";
    content += "Saved: " + getTimestampString() + "\n\n";
    content += lastResultText;

    if(saveToFile(fileName, content))
    {
        lastResultText += "\n\nSaved to file: " + fileName;
    }
    else
    {
        lastResultText += "\n\nFailed to save file.";
    }
}

// Returns the current screen name used in saved result files.
string App::currentScreenName() const
{
    if(currentScreen == HOME_SCREEN)
    {
        return "Home";
    }

    if(currentScreen == PRIME_SCREEN)
    {
        return "Primality Test";
    }

    if(currentScreen == RSA_SCREEN)
    {
        return "RSA";
    }

    if(currentScreen == DH_SCREEN)
    {
        return "Diffie-Hellman";
    }

    if(currentScreen == KEY_TOOLS_SCREEN)
    {
        return "Math Tools";
    }

    if(currentScreen == BENCHMARK_SCREEN)
    {
        return "Benchmarks";
    }

    return "Unknown";
}

// Parses textbox content into a non-negative long long with full-string validation.
bool App::stringToLongLong(const string& text, long long& value)
{
    int i;

    if(text.empty())
    {
        return false;
    }

    if(text.size() > MAX_INPUT_DIGITS)
    {
        return false;
    }

    i = 0;
    while(i < static_cast<int>(text.size()))
    {
        if(!isdigit(static_cast<unsigned char>(text[i])))
        {
            return false;
        }

        i++;
    }

    stringstream ss(text);
    ss >> value;

    return !ss.fail();
}
