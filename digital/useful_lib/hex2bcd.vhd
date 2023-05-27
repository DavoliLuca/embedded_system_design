library ieee ;
use ieee.std_logic_1164.all ;
use ieee.std_logic_unsigned.all ;

entity hex2bcd is
    port ( hex_in  : in  std_logic_vector (7 downto 0) ;
           bcd_hun : out std_logic_vector (3 downto 0) ;
           bcd_ten : out std_logic_vector (3 downto 0) ;
           bcd_uni : out std_logic_vector (3 downto 0) ) ;
end hex2bcd ;

architecture arc_hex2bcd of hex2bcd is
begin
process ( hex_in )
    variable hex_src : std_logic_vector (7 downto 0) ;
    variable bcd     : std_logic_vector (11 downto 0) ;
begin
    hex_src := hex_in ;
    bcd     := (others => '0') ;

    for i in 0 to 7 loop
        if bcd(3 downto 0) > "0100" then
            bcd(3 downto 0) := bcd(3 downto 0) + "0011" ;
        end if ;
        if bcd(7 downto 4) > "0100" then
            bcd(7 downto 4) := bcd(7 downto 4) + "0011" ;
        end if ;
        if bcd(11 downto 8) > "0100" then
            bcd(11 downto 8) := bcd(11 downto 8) + "0011" ;
        end if ;

        bcd := bcd(10 downto 0) & hex_src(7) ; -- shift bcd + 1 new entry
        hex_src := hex_src(6 downto 0) & '0' ; -- shift src + pad with 0
    end loop ;

    bcd_hun <= bcd(11 downto 8) ;
    bcd_ten <= bcd(7  downto 4) ;
    bcd_uni <= bcd(3  downto 0) ;

end process ;
end arc_hex2bcd ;