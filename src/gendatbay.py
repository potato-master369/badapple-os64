from PIL import Image

# Standard 4x4 Bayer Dithering Matrix
BAYER_MATRIX = [
    [  0, 128,  32, 160],
    [192,  64, 224,  96],
    [ 48, 176,  16, 144],
    [240, 112, 208,  80]
]

with open("badapple.dat", "wb") as f:
    for i in range(1, 4644):
        print(f"Processing frame: {i}/4643")

        # 1. Open image and convert to Grayscale ('L')
        img = Image.open(f"out/{i}.png").convert("L")
        width, height = img.size
        
        # Load pixels into a modifiable pixel map
        pix = img.load()
        
        # 2. Apply Ordered Dithering manually
        for y in range(height):
            for x in range(width):
                # Get the threshold value from the 4x4 matrix based on pixel coordinates
                threshold = BAYER_MATRIX[y % 4][x % 4]
                
                # If pixel is brighter than the threshold, make it white (255), else black (0)
                pix[x, y] = 255 if pix[x, y] > threshold else 0

        # 3. Convert the post-processed image to 1-bit without Pillow's default dither
        img = img.convert("1", dither=Image.Dither.NONE)
        
        # [Your existing code to pack pixels into bytes goes here...]
        pix = img.load()

        for y in range(height):

            current_byte = 0

            bit_count = 0


            for x in range(width):


                bit = 0 if pix[x, y] == 0 else 1;



                current_byte = (current_byte << 1) | bit

                bit_count += 1



                if bit_count == 8:

                    try:

                        f.write(bytes([current_byte]))

                    except Exception as e:

                        print(current_byte)

                        exit()

                    current_byte = 0

                    bit_count = 0



            if bit_count > 0:

                current_byte = current_byte << (8 - bit_count)

                f.write(bytes([current_byte]))
