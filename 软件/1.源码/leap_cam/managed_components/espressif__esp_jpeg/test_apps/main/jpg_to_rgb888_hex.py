from PIL import Image


def jpg_to_rgb888_hex_c_array(input_filename: str, output_filename: str) -> str:
    """
    Convert a .jpg file to RGB888 hex data and format it as a C-style array.

    Parameters:
    input_filename (str): The path to the JPEG file.

    Returns:
    str: A string representing the RGB888 hex data formatted as a C array.
    """
    # Open the image file
    with Image.open(input_filename) as img:
        # Ensure the image is in RGB mode
        rgb_img = img.convert("RGB")

        # Get image dimensions
        width, height = rgb_img.size

        # List to store hex values as C-style entries
        hex_data = []

        # Iterate over each pixel to get RGB values
        for y in range(height):
            for x in range(width):
                r, g, b = rgb_img.getpixel((x, y))
                # Format each RGB value as C-style hex (e.g., 0xRRGGBB)
                hex_data.append(f"0x{r:02X}{g:02X}{b:02X}")

    # Format as a C-style array with line breaks for readability
    hex_array = ",\n    ".join(hex_data)
    c_array = f"unsigned int image_data[{width * height}] = {{\n    {hex_array}\n}};"

    # Write the C array to the output file
    with open(output_filename, "w") as file:
        file.write(c_array)

    print(f"C-style RGB888 hex array saved to {output_filename}")

    return c_array


def main():
    """
    Main function to convert a JPEG file to an RGB888 C-style hex array.

    Instructions:
    1. Replace 'input.jpg' with the path to your JPEG file.
    2. Run the script to get the C-style array output.
    """
    # Input JPEG file path
    input_filename = "usb_camera.jpg"  # Replace with your JPEG file path

    # Output file path for the C array
    output_filename = "output_array.c"  # Specify your desired output filename

    # Convert JPEG to C-style RGB888 hex array
    jpg_to_rgb888_hex_c_array(input_filename, output_filename)


if __name__ == "__main__":
    main()
