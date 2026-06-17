from PIL import Image


with open("badapple.dat", "wb") as f:
    for i in range(1, 4644):
        print(f"Processing frame: {i}/4643")
            
        img = Image.open(f"out/{i}.png").convert("L").convert("1") 
        pix = img.load()
        width, height = img.size

        for y in range(height):
            current_byte = 0
            bit_count = 0
            
            for x in range(width):

                bit = 1 if pix[x, y] == 255 else 0;
                

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

print("Done!")
