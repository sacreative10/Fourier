import svgwrite
from svgpathtools import parse_path


def svg_file_to_complex(file_path):
    with open(file_path, "r") as svg_file:
        svg_content = svg_file.read()

    path = parse_path(svg_content)
    complex_coordinates = []

    for segment in path:
        if segment.start is not None:
            complex_coordinates.append(complex(segment.start.real, segment.start.imag))
        if segment.end is not None:
            complex_coordinates.append(complex(segment.end.real, segment.end.imag))

    return complex_coordinates


# Example usage
svg_file_path = "res/musical-note-music.svg"
complex_coordinates = svg_file_to_complex(svg_file_path)
print(complex_coordinates)
