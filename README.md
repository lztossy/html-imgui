# HTML -> ImGui

An experimental HTML-to-ImGui renderer. It allows you to load HTML files and render HTML elements such as `<h1>`, `<p>`, `<div>`, `<button>`, and `<slider>` using the Dear ImGui framework. The project is still under development

## Features

- Load and render HTML files into ImGui.
- Supports rendering:
  - Headers (`<h1>`)
  - Paragraphs (`<p>`)
  - Divs (`<div>`)
  - Buttons (`<button>`)
  - Sliders (`<slider>`)
- Dynamic rendering of HTML content from external files.
- ImGui interaction with buttons and sliders.

## Installation

### Prerequisites

- **C++ Compiler**: Ensure you have a C++ compiler installed.
- **CMake**: Required for generating build files.
- **DirectX SDK**: Ensure you have the necessary DirectX dependencies.
- **Dear ImGui**: This project uses the Dear ImGui framework for rendering.

### Setup Instructions

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/lztossy/html-imgui.git
   cd html-imgui
