target
    memsize 8
    byteorder big
    pointersize 32
    wordsize 32
    charset "latin1"
    float "ieee754";

export bits32 \212\029\140\217\143\000\178\004\233\128\t\152\236\248B~;

section "data" { align 1; }

section "data" { sym@Cmm_private_global_area: }

section "data"
{
    sym@@212@029@140@217@143@000@178@004@233@128@t@152@236@248B@:
}

section "data" { bits8[0::bits32]; }

section "text"
{
    sym@p()
    {
        $r31 = ($r31+-24);
        ;
        ;
        $t1 = $r30;
        ;
        initialize continuations:l4:
        ii = 1;
        ij = 2;
        ik = 0;
        il = 0;
        $c0 when %ge[32](ij, 2) = sym@join@l6;
        join:l12:
        $c0 = sym@join@l7;
        join:l6:
        ik = 3;
        il = 4;
        $c0 = sym@join@l7;
        join:l7:
        iitotal = (((ii+ij)+ik)+il);
        ;
        $r0 = iitotal;
        $t1 = $t1;
        ;
        $r31 = ($r31+24);
        $c0 = $t1;
    }
}

section "text" {  }

