#include "utils.h"

// exclude satellites ----------------------------------------------------------
static void ExSats(const char *exsats, unsigned char *exsats_opt)
{
    char buff[1024];
    char *id;
    int sat;

    for (int i=0;i<MAXSAT;i++) exsats_opt[i]=0;
    if (exsats[0]!='\0') {
        strcpy(buff,exsats);
        for (char *p=strtok(buff," ");p;p=strtok(NULL," ")) {
            if (*p=='+') id=p+1; else id=p;
            if (!(sat=satid2no(id))) continue;
            exsats_opt[sat-1]=*p=='+'?2:1;
        }
    }
}
// fill antenna position -------------------------------------------------------
static void FillAntPos(int antpostype, const double *antpos, int *postype_opt,
                       double *pos_opt)
{
    double pos[3];

    if (antpostype==0) { /* lat/lon/hgt */
        *postype_opt=0;
        pos[0]=antpos[0]*D2R;
        pos[1]=antpos[1]*D2R;
        pos[2]=antpos[2];
        pos2ecef(pos,pos_opt);
    }
    else if (antpostype==1) { /* xyz-ecef */
        *postype_opt=0;
        pos_opt[0]=antpos[0];
        pos_opt[1]=antpos[1];
        pos_opt[2]=antpos[2];
    }
    else {
        *postype_opt=antpostype-1;
    }
}
// initialize system options ---------------------------------------------------
void InitializeSysOpts(prcopt_t *prcopt, solopt_t *solopt, filopt_t *filopt)
{
    *prcopt=prcopt_default;
    *solopt=solopt_default;
    *filopt=filopt_t{};

    prcopt->mode=2;                 // pos1-posmode     (0:single,1:dgps,2:kinematic,3:static,4:static-start,5:movingbase,6:fixed,7:ppp-kine,8:ppp-static,9:ppp-fixed)
    prcopt->nf=2;                   // pos1-frequency   (1:l1,2:l1+l2,3:l1+l2+l5,4:l1+l5)
    prcopt->soltype=0;              // pos1-soltype     (0:forward,1:backward,2:combined)
    prcopt->elmin=15.0*D2R;         // pos1-elmask      (rad)
    prcopt->snrmask.ena[0]=1;       // pos1-snrmask_r   (0:off,1:on)
    prcopt->snrmask.ena[1]=0;       // pos1-snrmask_b   (0:off,1:on)
    for (int nf=0;nf<NFREQ;nf++) {  // pos1-snrmask_LN
        for (int j=0;j<9;j++) {
            prcopt->snrmask.mask[nf][j]=35.0;
        }
    }
    prcopt->dynamics=1;             // pos1-dynamics    (0:off,1:on)
    prcopt->tidecorr=0;             // pos1-tidecorr    (0:off,1:on,2:otl)
    prcopt->ionoopt=1;              // pos1-ionoopt     (0:off,1:brdc,2:sbas,3:dual-freq,4:est-stec,5:ionex-tec,6:qzs-brdc,7:qzs-lex,8:stec)
    prcopt->tropopt=2;              // pos1-tropopt     (0:off,1:saas,2:sbas,3:est-ztd,4:est-ztdgrad,5:ztd)
    prcopt->sateph=0;               // pos1-sateph      (0:brdc,1:precise,2:brdc+sbas,3:brdc+ssrapc,4:brdc+ssrcom)
    prcopt->posopt[0]=0;            // pos1-posopt1     (0:off,1:on)
    prcopt->posopt[1]=0;            // pos1-posopt2     (0:off,1:on)
    prcopt->posopt[2]=0;            // pos1-posopt3     (0:off,1:on,2:precise)
    prcopt->posopt[3]=0;            // pos1-posopt4     (0:off,1:on)
    prcopt->posopt[4]=0;            // pos1-posopt5     (0:off,1:on)
    prcopt->posopt[5]=0;            // pos1-posopt6     (0:off,1:on)
    char exsats[1024]="\0";         // pos1-exclsats    (prn ...)
    ExSats(exsats,prcopt->exsats);
    prcopt->navsys=63;              // pos1-navsys      (1:gps+2:sbas+4:glo+8:gal+16:qzs+32:comp)

    prcopt->modear=3;               // pos2-armode      (0:off,1:continuous,2:instantaneous,3:fix-and-hold)
    prcopt->glomodear=1;            // pos2-gloarmode   (0:off,1:on,2:autocal,3:fix-and-hold)
    prcopt->bdsmodear=1;            // pos2-bdsarmode   (0:off,1:on)
    prcopt->arfilter=1;             // pos2-arfilter    (0:off,1:on)
    prcopt->thresar[0]=3.0;         // pos2-arthres
    prcopt->thresar[1]=0.1;         // pos2-arthres1
    prcopt->thresar[2]=0.25;        // pos2-arthres2
    prcopt->thresar[3]=0.1;         // pos2-arthres3
    prcopt->thresar[4]=0.05;        // pos2-arthres4
    prcopt->minlock=0;              // pos2-arlockcnt
    prcopt->minfixsats=4;           // pos2-minfixsats
    prcopt->minholdsats=5;          // pos2-minholdsats
    prcopt->rcvstds=0;              // pos2-rcvstds     (0:off,1:on)
    prcopt->elmaskar=0.0*D2R;       // pos2-arelmask    (rad)
    prcopt->minfix=100;             // pos2-arminfix
    prcopt->armaxiter=1;            // pos2-armaxiter
    prcopt->elmaskhold=0.0*D2R;     // pos2-elmaskhold  (rad)
    prcopt->maxout=100;             // pos2-aroutcnt
    prcopt->maxtdiff=60.0;          // pos2-maxage      (s)
    prcopt->syncsol=0;              // pos2-syncsol     (0:off,1:on)
    prcopt->thresslip=0.05;         // pos2-slipthres   (m)
    prcopt->maxinno=1000.0;         // pos2-rejionno    (m)
    prcopt->maxgdop=30.0;           // pos2-rejgdop
    prcopt->niter=1;                // pos2-niter
    prcopt->baseline[0]=0.0;        // pos2-baselen     (m)
    prcopt->baseline[1]=0.0;        // pos2-basesig     (m)

    prcopt->base_multi_epoch=1;     // base-multi_epoch (0:off,1:on)

    prcopt->residual_mode=1;        // resid-mode       (0:off,1:on)

    solopt->posf=0;                 // out-solformat    (0:llh,1:xyz,2:enu,3:nmea)
    solopt->outhead=1;              // out-outhead      (0:off,1:on)
    solopt->outopt=0;               // out-outopt       (0:off,1:on)
    solopt->outvel=0;               // out-outvel       (0:off,1:on)
    solopt->times=0;                // out-timesys      (0:gpst,1:utc,2:jst)
    solopt->timef=1;                // out-timeform     (0:tow,1:hms)
    solopt->timeu=3;                // out-timendec
    solopt->degf=0;                 // out-degform      (0:deg,1:dms)
    solopt->sep[0]='\0';            // out-fieldsep
    prcopt->outsingle=1;            // out-outsingle    (0:off,1:on)
    solopt->height=0;               // out-height       (0:ellipsoidal,1:geodetic)
    solopt->geoid=0;                // out-geoid        (0:internal,1:egm96,2:egm08_2.5,3:egm08_1,4:gsi2000)
    solopt->solstatic=0;            // out-solstatic    (0:all,1:single)
    solopt->nmeaintv[0]=0.0;        // out-nmeaintv1    (s)
    solopt->nmeaintv[1]=0.0;        // out-nmeaintv2    (s)
    solopt->sstat=0;                // out-outstat      (0:off,1:state,2:residual)
    solopt->out_additional_info=0;  // out-addit_info   (0:off,1:on)

    prcopt->eratio[0]=300.0;        // stats-eratio1
    prcopt->eratio[1]=100.0;        // stats-eratio2
    prcopt->err[1]=0.003;           // stats-errphase   (m)
    prcopt->err[2]=0.003;           // stats-errphaseel (m)
    prcopt->err[3]=0.0;             // stats-errphasebl (m/10km)
    prcopt->err[4]=10.0;            // stats-errdoppler (Hz)
    prcopt->std[0]=30.0;            // stats-stdbias    (m)
    prcopt->std[1]=0.03;            // stats-stdiono    (m)
    prcopt->std[2]=0.3;             // stats-stdtrop    (m)
    prcopt->prn[3]=1.0;             // stats-prnaccelh  (m/s^2)
    prcopt->prn[4]=1.0;             // stats-prnaccelv  (m/s^2)
    prcopt->prn[0]=0.0001;          // stats-prnbias    (m)
    prcopt->prn[1]=0.001;           // stats-prniono    (m)
    prcopt->prn[2]=0.0001;          // stats-prntrop    (m)
    prcopt->prn[5]=0.0;             // stats-prnpos     (m)
    prcopt->sclkstab=5e-012;        // stats-clkstab    (s/s)

    int antrovpostype=0;            // ant1-postype     (0:llh,1:xyz,2:single,3:posfile,4:rinexhead,5:rtcm,6:raw)
    double antrovpos[3]={
        0.0,                        // ant1-pos1        (deg|m)
        0.0,                        // ant1-pos2        (deg|m)
        0.0                         // ant1-pos3        (m|m)
    };
    FillAntPos(antrovpostype,antrovpos,&prcopt->rovpos,prcopt->ru);
    prcopt->anttype[0][0]='\0';     // ant1-anttype
    prcopt->antdel[0][0]=0.0;       // ant1-antdele     (m)
    prcopt->antdel[0][1]=0.0;       // ant1-antdeln     (m)
    prcopt->antdel[0][2]=0.0;       // ant1-antdelu     (m)

    int antbasepostype=4;           // ant2-postype     (0:llh,1:xyz,2:single,3:posfile,4:rinexhead,5:rtcm,6:raw)
    double antbasepos[3]={
        0.0,                        // ant2-pos1        (deg|m)
        0.0,                        // ant2-pos2        (deg|m)
        0.0                         // ant2-pos3        (m|m)
    };
    FillAntPos(antbasepostype,antbasepos,&prcopt->refpos,prcopt->rb);
    prcopt->anttype[1][0]='\0';     // ant2-anttype
    prcopt->antdel[1][0]=0.0;       // ant2-antdele     (m)
    prcopt->antdel[1][1]=0.0;       // ant2-antdeln     (m)
    prcopt->antdel[1][2]=0.0;       // ant2-antdelu     (m)
    prcopt->maxaveep=0;             // ant2-maxaveep
    prcopt->initrst=0;              // ant2-initrst     (0:off,1:on)

    prcopt->intpref=0;              // misc-timeinterp  (0:off,1:on)
    prcopt->sbassatsel=0;           // misc-sbasatsel   (0:all)
    prcopt->rnxopt[0][0]='\0';      // misc-rnxopt1
    prcopt->rnxopt[1][0]='\0';      // misc-rnxopt2
    prcopt->pppopt[0]='\0';         // misc-pppopt

    filopt->satantp[0]='\0';        // file-satantfile
    filopt->rcvantp[0]='\0';        // file-rcvantfile
    filopt->stapos[0]='\0';         // file-staposfile
    filopt->geoid[0]='\0';          // file-geoidfile
    filopt->iono[0]='\0';           // file-ionofile
    filopt->dcb[0]='\0';            // file-dcbfile
    filopt->eop[0]='\0';            // file-eopfile
    filopt->blq[0]='\0';            // file-blqfile
    filopt->tempdir[0]='\0';        // file-tempdir
    filopt->geexe[0]='\0';          // file-geexefile
    filopt->solstat[0]='\0';        // file-solstatfile
    filopt->trace[0]='\0';          // file-tracefile
}
